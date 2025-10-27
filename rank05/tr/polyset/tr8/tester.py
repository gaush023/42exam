#!/usr/bin/env python3
"""Strict tester for the Polyset assignment.

The script compiles the student's implementation together with a test harness
(`polyset_tester.cpp`) and executes the resulting binary. Compilation and test
failures are reported with detailed diagnostics.
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
HARNESS = ROOT / "polyset_tester.cpp"
BINARY = ROOT / "polyset_tester"
SOURCES = [
    "array_bag.cpp",
    "tree_bag.cpp",
    "searchable_array_bag.cpp",
    "searchable_tree_bag.cpp",
    "set.cpp",
    HARNESS.name,
]


def compile_program(compiler: str = "c++", std: str = "c++11") -> None:
    """Compile the student sources and the test harness.

    Raises ``RuntimeError`` if compilation fails.
    """

    cmd = [
        compiler,
        "-Wall",
        "-Wextra",
        "-Werror",
        "-pedantic",
        f"-std={std}",
        *SOURCES,
        "-o",
        str(BINARY),
    ]
    proc = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    if proc.returncode != 0:
        raise RuntimeError(
            "Compilation failed with command:\n"
            + " ".join(cmd)
            + "\n--- stdout ---\n"
            + proc.stdout
            + "\n--- stderr ---\n"
            + proc.stderr
        )


def run_binary() -> subprocess.CompletedProcess:
    """Run the compiled binary and return the completed process."""

    if not BINARY.exists():
        raise FileNotFoundError(
            f"Binary {BINARY} does not exist. Compile the project first."
        )
    return subprocess.run(
        [str(BINARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Run the Polyset tester")
    parser.add_argument(
        "--compiler",
        default="c++",
        help="C++ compiler to use (default: c++)",
    )
    parser.add_argument(
        "--std",
        default="c++11",
        help="C++ language standard flag to pass to the compiler",
    )
    parser.add_argument(
        "--skip-compile",
        action="store_true",
        help="Skip the compilation step and only run the existing binary",
    )
    parser.add_argument(
        "--keep-binary",
        action="store_true",
        help="Do not delete the compiled binary after running the tests",
    )
    args = parser.parse_args(argv)

    if not args.skip_compile:
        try:
            compile_program(args.compiler, args.std)
        except RuntimeError as exc:
            print(exc, file=sys.stderr)
            return 1

    try:
        proc = run_binary()
    except FileNotFoundError as exc:
        print(exc, file=sys.stderr)
        return 1

    sys.stdout.write(proc.stdout)
    if proc.stderr:
        sys.stderr.write(proc.stderr)

    if proc.returncode != 0:
        if not args.keep_binary and BINARY.exists():
            BINARY.unlink()
        return proc.returncode

    if not args.keep_binary and BINARY.exists():
        BINARY.unlink()

    return 0


if __name__ == "__main__":
    sys.exit(main())
