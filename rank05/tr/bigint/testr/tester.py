#!/usr/bin/env python3
import subprocess
import sys
import argparse
from pathlib import Path

ROOT = Path(__file__).resolve().parent
BINARY = ROOT / "a.out"

def build(strict=False):
    # 普通テスターと厳しいテスターを切り替え
    tester = "tester_main.cpp" if strict else "main.cpp"
    cmd = ["c++", "-Wall", "-Wextra", "-Werror", "-std=c++98",
           "bigint.cpp", tester, "-o", str(BINARY)]
    print("=== build ===")
    print(" ".join(cmd))
    proc = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if proc.returncode != 0:
        sys.stderr.write("❌ コンパイル失敗\n")
        sys.stderr.write(proc.stderr.decode(errors="replace"))
        sys.exit(1)
    else:
        out = proc.stderr.decode(errors="replace")
        if out.strip():
            print("--- warnings ---")
            print(out)
        print("✅ build success")

def run():
    print("\n=== run tests ===")
    proc = subprocess.run([str(BINARY)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    sys.stdout.write(proc.stdout.decode(errors="replace"))
    sys.stderr.write(proc.stderr.decode(errors="replace"))

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--strict", action="store_true",
                        help="厳しいテスターを使用する")
    args = parser.parse_args()

    build(strict=args.strict)
    run()
