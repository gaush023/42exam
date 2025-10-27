#!/usr/bin/env python3
import subprocess
import sys

# 課題要件 (各行は width 文字固定、死セルはスペースで出力) に合わせた期待出力
EXPECTED = {
    "サンプル1": (
        "     \n"
        " OOO \n"
        " O O \n"
        " OOO \n"
        "     \n"
    ),
    "サンプル2": (
        "          \n"
        " O   OOO  \n"
        " O     O  \n"
        " OOO  O   \n"
        "  O  OOO  \n"
        "          \n"
    ),
    "縦棒 iteration=0": (
        " O \n"
        " O \n"
        " O \n"
    ),
    "縦棒 iteration=1": (
        "   \n"
        "OOO\n"
        "   \n"
    ),
    "縦棒 iteration=2": (
        " O \n"
        " O \n"
        " O \n"
    ),
    "空入力": (
        "     \n"
        "     \n"
        "     \n"
        "     \n"
        "     \n"
    ),
    "1x1 生存セル": "O\n",
    "1x1 死亡セル": " \n",
    "大きめボード": (
        "                    \n"
        "                    \n"
        "     O              \n"
        "     O              \n"
        "     O              \n"
        "     OOOOOO         \n"
        "                    \n"
        "                    \n"
        "                    \n"
        "                    \n"
    ),
    "ペン上下テスト": (
        "OOOO \n"
        "   O \n"
        "   O \n"
        "   O \n"
        "     \n"
    ),
    "長期イテレーション": (
        "   \n"
        "OOO\n"
        "   \n"
    ),
    "不正入力混入": (
        " O   \n"
        " O   \n"
        " O   \n"
        "     \n"
        "     \n"
    ),
}


def compile_program():
    print("=== コンパイル開始 ===")
    cmd = "cc -Wall -Wextra -Werror *.c -o life"
    result = subprocess.run(
        cmd,
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )
    if result.returncode != 0:
        print("✘ コンパイル失敗")
        if result.stdout:
            print(result.stdout.decode(errors="replace"))
        if result.stderr:
            print(result.stderr.decode(errors="replace"))
        sys.exit(1)
    else:
        print("✔ コンパイル成功\n")


def run_case(name, args, stdin=""):
    print(f"\n=== {name} ===")
    proc = subprocess.run(
        ["./life"] + list(map(str, args)),
        input=stdin.encode(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    out = proc.stdout.decode(errors="replace")
    expected = EXPECTED.get(name)

    print("=== stdout ===")
    print(out if out else "(empty)")

    if expected is not None:
        if out == expected:
            print("✔ OK (期待通り)")
        else:
            print("✘ NG (期待と不一致)")
            print("--- 期待値 ---")
            print(repr(expected))
            print("--- 実際 ---")
            print(repr(out))
    else:
        print("(期待値未設定)")


def main():
    compile_program()
    run_case("サンプル1", [5, 5, 0], "sdxddssaaww")
    run_case("サンプル2", [10, 6, 0], "sdxssdswdxddddsxaadwxwdxwaa")
    run_case("縦棒 iteration=0", [3, 3, 0], "dxss")
    run_case("縦棒 iteration=1", [3, 3, 1], "dxss")
    run_case("縦棒 iteration=2", [3, 3, 2], "dxss")
    run_case("空入力", [5, 5, 0], "")
    run_case("1x1 生存セル", [1, 1, 0], "x")
    run_case("1x1 死亡セル", [1, 1, 1], "x")
    run_case("大きめボード", [20, 10, 0], "s"*5 + "d"*10 + "x" + "a"*5 + "w"*3)
    run_case("ペン上下テスト", [5, 5, 0], "xdddxxsss")
    run_case("長期イテレーション", [3, 3, 5], "dxss")
    run_case("不正入力混入", [5, 5, 0], "dxssQZZ!?")


if __name__ == "__main__":
    main()

