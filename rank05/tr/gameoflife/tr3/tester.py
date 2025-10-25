#!/usr/bin/env python3
import subprocess
import sys

def cat_e(s: str) -> str:
    """ 行末に $ を付け、空白もそのまま表示 """
    lines = s.splitlines()
    return "\n".join(line + "$" for line in lines)

def compile_program():
    print("=== コンパイル開始 ===")
    cmd = "cc -Wall -Wextra -Werror *.c -o a.out"
    result = subprocess.run(
        cmd,
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
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

def run_case(name, w, h, r, keystrokes: str):
    print(f"\n=== {name} ===")
    print(f"cmd: ./a.out {w} {h} {r}")
    print(f"keys: {keystrokes!r}")

    try:
        proc = subprocess.run(
            ["./a.out", str(w), str(h), str(r)],
            input=keystrokes.encode(),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )
    except FileNotFoundError:
        print("!! ./a.out が見つかりません。")
        return

    out = proc.stdout.decode(errors="replace")
    err = proc.stderr.decode(errors="replace")

    print("---- stdout ----")
    print(cat_e(out))
    if err.strip():
        print("---- stderr ----")
        print(err)

def path_moves(from_i, from_j, to_i, to_j):
    """(i,j) から (to_i,to_j) へ最短の wasd 移動列を返す"""
    s = []
    di = to_i - from_i
    dj = to_j - from_j
    s.extend('s' * max(0, di))
    s.extend('w' * max(0, -di))
    s.extend('d' * max(0, dj))
    s.extend('a' * max(0, -dj))
    return "".join(s), to_i, to_j

def draw_cells(cells, w, h, start_i=0, start_j=0, toggle_off=False):
    """
    cells: [(i,j), ...] を順になぞって 'x' で描きながら到達する入力列を作る。
    - toggle_off=True なら最後に 'x' を追加して描画OFFに戻す
    """
    keys = []
    cur_i, cur_j = start_i, start_j
    keys.append('x')  # 描画ON
    for (ti, tj) in cells:
        mv, cur_i, cur_j = path_moves(cur_i, cur_j, ti, tj)
        keys.append(mv)
    if toggle_off:
        keys.append('x')
    return "".join(keys)

if __name__ == "__main__":
    compile_program()

    # 1) サンプルケース
    run_case("sample 5x5 / r=0", 5, 5, 0, "sdxddssaaww")
    run_case("sample 10x6 / r=0", 10, 6, 0, "sdxssdswdxddddsxaadwxwdxwaa")

    # 2) ブリンカー（縦棒 → 横棒）
    blinker_cells = [(0,1), (1,1), (2,1)]
    keys_blinker = draw_cells(blinker_cells, 3, 3, toggle_off=True)
    run_case("blinker 3x3 / r=0", 3, 3, 0, keys_blinker)
    run_case("blinker 3x3 / r=1", 3, 3, 1, keys_blinker)
    run_case("blinker 3x3 / r=2", 3, 3, 2, keys_blinker)

    # 3) 手打ちの短い例
    run_case("manual dxss / 3x3 / r=0", 3, 3, 0, "dxss")
    run_case("manual dxss / 3x3 / r=1", 3, 3, 1, "dxss")
    run_case("manual dxss / 3x3 / r=2", 3, 3, 2, "dxss")

    # 4) グライダー（5x5 / 4世代）
    glider = [(0,1), (1,2), (2,0), (2,1), (2,2)]
    keys_glider = draw_cells(glider, 5, 5, toggle_off=True)
    run_case("glider 5x5 / r=0", 5, 5, 0, keys_glider)
    run_case("glider 5x5 / r=1", 5, 5, 1, keys_glider)
    run_case("glider 5x5 / r=4", 5, 5, 4, keys_glider)

