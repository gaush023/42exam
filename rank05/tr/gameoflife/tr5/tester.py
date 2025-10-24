import subprocess
import shlex
import sys

def build():
    cmd = ["cc", "-Wall", "-Wextra", "-Werror", "gameoflife.c", "-o", "a.out"]
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

def cat_e(s: str) -> str:
    """行末に $ を付与して可視化"""
    lines = s.splitlines()
    return "\n".join(line + "$" for line in lines)

def run_error_case(name, args, keystrokes=""):
    print(f"\n=== {name} ===")
    print(f"cmd: ./a.out {' '.join(args)}")
    try:
        proc = subprocess.run(
            ["./a.out"] + args,
            input=keystrokes.encode(),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )
    except FileNotFoundError:
        print("!! ./a.out が見つかりません。先に build() を確認してください。")
        return

    out = proc.stdout.decode(errors="replace")
    err = proc.stderr.decode(errors="replace")

    print("---- stdout ----")
    print(cat_e(out))
    if err.strip():
        print("---- stderr ----")
        print(err)

    if proc.returncode == 0:
        print("❌ 本来はエラーで終了すべきなのに returncode=0")
    else:
        print(f"✅ returncode={proc.returncode} (エラー終了を確認)")


def run_case(name, w, h, r, keystrokes: str, expected: str = None):
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
        print("!! ./a.out が見つかりません。先に build() を確認してください。")
        return

    out = proc.stdout.decode(errors="replace")
    err = proc.stderr.decode(errors="replace")

    print("---- stdout ----")
    print(cat_e(out))
    if err.strip():
        print("---- stderr ----")
        print(err)

    if proc.returncode != 0:
        print(f"❌ returncode={proc.returncode}")
    else:
        print("✅ returncode=0")

    if expected is not None:
        exp = cat_e(expected)
        if cat_e(out) != exp:
            print("❌ 出力不一致")
            print("期待:")
            print(exp)
        else:
            print("✅ 出力一致")

def path_moves(from_i, from_j, to_i, to_j):
    """(i,j) → (to_i,to_j) 最短wasd移動列"""
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
    cells: [(i,j), ...] を順になぞって 'x' で描く入力列を作成
    - toggle_off=True なら最後に 'x' で描画OFF
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
    build()

    # case1: 入力例
    run_case(
        "case: 長い入力列 / 10x6 / 0世代",
        10, 6, 0,
        "sdxssdswdxddddsxaadwxwdxwaa"
    )

    # case2: ブリンカー
    blinker_cells = [(0,1), (1,1), (2,1)]
    keys_blinker = draw_cells(blinker_cells, 3, 3, toggle_off=True)
    run_case("blinker 3x3 / r=1", 3, 3, 1, keys_blinker)

    # case3: 手打ち例
    run_case("dxss 3x3 / r=1", 3, 3, 1, "dxss")

    # case4: グライダー
    glider = [(0,1), (1,2), (2,0), (2,1), (2,2)]
    keys_glider = draw_cells(glider, 5, 5, toggle_off=True)
    run_case("glider 5x5 / r=4", 5, 5, 4, keys_glider)
    run_error_case("引数不足", [])
    run_error_case("引数が文字列", ["a", "b", "c"])
    run_error_case("幅高さが0", ["0", "0", "1"])
