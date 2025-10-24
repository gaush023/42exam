#!/usr/bin/env python3
"""Utility script to test a BSQ implementation.

The tester can optionally build ``bsq`` from ``main.c`` and ``bsq.c`` and then
run a collection of randomly generated maps against the executable.  For each
case, the script computes the expected BSQ result with a reference
implementation and verifies that the program prints the same filled map.
"""

from __future__ import annotations

import argparse
import random
import subprocess
import sys
from pathlib import Path
from typing import Iterable, List, Sequence, NamedTuple

ROOT = Path(__file__).resolve().parent
DEFAULT_BINARY = ROOT / "bsq"


def compile_program(binary: Path = DEFAULT_BINARY) -> None:
    """Compile ``main.c`` and ``bsq.c`` into ``binary`` using ``cc``.

    Raises ``RuntimeError`` if compilation fails.
    """

    sources = ["main.c", "bsq.c"]
    cmd = [
        "cc",
        "-Wall",
        "-Wextra",
        "-Werror",
        "-std=c99",
        *sources,
        "-o",
        str(binary),
    ]
    proc = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    if proc.returncode != 0:
        raise RuntimeError(
            "Compilation failed:\n" f"stdout:\n{proc.stdout}\n" f"stderr:\n{proc.stderr}"
        )


class ParsedMap(NamedTuple):
    height: int
    empty: str
    obstacle: str
    full: str
    grid: List[str]



def parse_map(lines: Sequence[str]) -> ParsedMap:
    """Parse a BSQ map from ``lines``.

    Supports both the "``height empty obstacle full``" header format (used in
    this repository) and the classic "``<height><empty><obstacle><full>``"
    format used in some 42 subjects.
    """

    if not lines:
        raise ValueError("Map is empty")

    header = lines[0].strip()
    tokens = header.split()
    if len(tokens) == 4:
        height = int(tokens[0])
        empty, obstacle, full = tokens[1:4]
    elif len(tokens) == 1 and len(tokens[0]) >= 4:
        raw = tokens[0]
        height = int(raw[:-3])
        empty, obstacle, full = raw[-3], raw[-2], raw[-1]
    else:
        raise ValueError(f"Unsupported header format: {header!r}")

    grid = [line.rstrip("\n") for line in lines[1:]]
    if len(grid) != height:
        raise ValueError(
            f"Header declares {height} rows but map has {len(grid)} rows"
        )
    if not grid:
        return ParsedMap(height, empty, obstacle, full, grid)

    width = len(grid[0])
    for row in grid:
        if len(row) != width:
            raise ValueError("Map rows do not have consistent width")
        for ch in row:
            if ch not in (empty, obstacle):
                raise ValueError(f"Invalid character {ch!r} in map")

    return ParsedMap(height, empty, obstacle, full, grid)


def solve_bsq(parsed: ParsedMap) -> List[str]:
    """Return the map grid with the largest square filled using ``full``."""

    height, empty, obstacle, full, grid = parsed
    if not grid:
        return []

    width = len(grid[0])
    dp = [[0] * width for _ in range(height)]
    best_size = 0
    best_i = 0
    best_j = 0

    for i in range(height):
        for j in range(width):
            if grid[i][j] == obstacle:
                dp[i][j] = 0
            else:
                if i == 0 or j == 0:
                    dp[i][j] = 1
                else:
                    dp[i][j] = 1 + min(dp[i - 1][j], dp[i - 1][j - 1], dp[i][j - 1])
                if dp[i][j] > best_size:
                    best_size = dp[i][j]
                    best_i = i - best_size + 1
                    best_j = j - best_size + 1

    result = [list(row) for row in grid]
    for di in range(best_size):
        for dj in range(best_size):
            result[best_i + di][best_j + dj] = full
    return ["".join(row) for row in result]


def generate_random_map(
    width: int,
    height: int,
    empty: str = ".",
    obstacle: str = "o",
    full: str = "B",
    obstacle_prob: float = 0.2,
) -> List[str]:
    """Generate a random BSQ map in string form."""

    header = f"{height} {empty} {obstacle} {full}"
    rows = []
    for _ in range(height):
        row = "".join(
            obstacle if random.random() < obstacle_prob else empty
            for _ in range(width)
        )
        rows.append(row)
    return [header, *rows]


def run_program(binary: Path, map_lines: Sequence[str]) -> subprocess.CompletedProcess:
    """Execute ``binary`` with ``map_lines`` fed on ``stdin``."""

    payload = "\n".join(map_lines) + "\n"
    return subprocess.run(
        [str(binary)],
        input=payload.encode(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )


def diff(expected: Sequence[str], actual: Sequence[str]) -> str:
    from difflib import unified_diff

    return "".join(
        unified_diff(
            [line + "\n" for line in expected],
            [line + "\n" for line in actual],
            fromfile="expected",
            tofile="actual",
        )
    )


def run_test_case(binary: Path, map_lines: Sequence[str], index: int) -> bool:
    parsed = parse_map(map_lines)
    expected = solve_bsq(parsed)
    proc = run_program(binary, map_lines)
    stdout_lines = proc.stdout.decode().splitlines()

    success = stdout_lines == expected
    print(f"[case {index}] {'OK' if success else 'FAIL'}")
    if proc.stderr:
        print(proc.stderr.decode(), file=sys.stderr)
    if not success:
        print(diff(expected, stdout_lines))
    return success


def main(argv: Iterable[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Test the bsq implementation")
    parser.add_argument(
        "--binary",
        type=Path,
        default=DEFAULT_BINARY,
        help="Path to the compiled bsq executable",
    )
    parser.add_argument(
        "--compile",
        action="store_true",
        help="Compile main.c and bsq.c before running tests",
    )
    parser.add_argument(
        "--cases",
        type=int,
        default=10,
        help="Number of random maps to generate",
    )
    parser.add_argument(
        "--min-size",
        type=int,
        default=5,
        help="Minimum width/height of generated maps",
    )
    parser.add_argument(
        "--max-size",
        type=int,
        default=20,
        help="Maximum width/height of generated maps",
    )
    parser.add_argument(
        "--obstacle-prob",
        type=float,
        default=0.2,
        help="Probability of an obstacle cell in generated maps",
    )
    parser.add_argument(
        "--seed",
        type=int,
        default=None,
        help="Random seed for reproducibility",
    )
    args = parser.parse_args(list(argv) if argv is not None else None)

    binary = args.binary
    if args.compile:
        try:
            compile_program(binary)
        except RuntimeError as exc:
            print(exc, file=sys.stderr)
            return 1

    if args.seed is not None:
        random.seed(args.seed)

    if not binary.exists():
        print(
            f"Executable {binary} does not exist. Use --compile or build it manually.",
            file=sys.stderr,
        )
        return 1

    if args.min_size < 1 or args.max_size < args.min_size:
        raise ValueError("Invalid size range for random maps")

    all_ok = True
    for case_index in range(1, args.cases + 1):
        width = random.randint(args.min_size, args.max_size)
        height = random.randint(args.min_size, args.max_size)
        map_lines = generate_random_map(
            width,
            height,
            obstacle_prob=args.obstacle_prob,
        )
        if not run_test_case(binary, map_lines, case_index):
            all_ok = False
    return 0 if all_ok else 1


if __name__ == "__main__":
    sys.exit(main())
