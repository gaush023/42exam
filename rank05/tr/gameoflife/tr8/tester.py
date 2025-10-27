#!/usr/bin/env python3
import subprocess
import sys


def compile_program():
    print("=== コンパイル開始 ===")
    # *.c を全部対象にする
    cmd = "cc -Wall -Wextra -Werror *.c -o life"
    result = subprocess.run(
        cmd,
        shell=True,               # ワイルドカード展開用
        stdout=subprocess.PIPE,   # 標準出力も取得
        stderr=subprocess.PIPE    # エラー出力も取得
    )

    if result.returncode != 0:
        print("✘ コンパイル失敗")
        if result.stdout:
            print("=== stdout ===")
            print(result.stdout.decode(errors="replace"))
        if result.stderr:
            print("=== stderr ===")
            print(result.stderr.decode(errors="replace"))
        sys.exit(1)  # ここで終了
    else:
        print("✔ コンパイル成功\n")


def run_case(name, args, stdin=""):
    print(f"\n=== {name} ===")
    print(f"cmd: ./life {' '.join(map(str, args))}")
    if stdin:
        print(f"stdin: {stdin!r}")
    proc = subprocess.run(
        ["./life"] + list(map(str, args)),
        input=stdin.encode(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    out = proc.stdout.decode(errors="replace")
    err = proc.stderr.decode(errors="replace")
    print("=== stdout ===")
    print(out if out else "(empty)")
    if err:
        print("=== stderr ===")
        print(err)


def main():
    compile_program()

    # サンプル
    run_case("サンプル1", [5, 5, 0], "sdxddssaaww")
    run_case("サンプル2", [10, 6, 0], "sdxssdswdxddddsxaadwxwdxwaa")
    run_case("縦棒 iteration=0", [3, 3, 0], "dxss")
    run_case("縦棒 iteration=1", [3, 3, 1], "dxss")
    run_case("縦棒 iteration=2", [3, 3, 2], "dxss")

    # エッジケース
    run_case("空入力", [5, 5, 0], "")
    run_case("1x1 生存セル", [1, 1, 0], "x")
    run_case("1x1 死亡セル", [1, 1, 1], "x")

    # 追加テスト
    run_case("大きめボード", [20, 10, 0], "s"*5 + "d"*10 + "x" + "a"*5 + "w"*3)
    run_case("ペン上下テスト", [5, 5, 0], "xdddxxsss")
    run_case("長期イテレーション", [3, 3, 5], "dxss")
    run_case("不正入力混入", [5, 5, 0], "dxssQZZ!?")


if __name__ == "__main__":
    main()

