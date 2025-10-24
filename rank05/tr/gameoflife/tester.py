# tester.py
import subprocess
import shlex

def cat_e(s: str) -> str:
    # 行末に $ を付け、空白もそのまま見えるようにする
    lines = s.splitlines()
    return "\n".join(line + "$" for line in lines)

def run_case(name, w, h, r, keystrokes: str):
    print(f"\n=== {name} ===")
    print(f"cmd: ./a.out {w} {h} {r}")
    print(f"keys: {keystrokes!r}")

    try:
        # ./a.out 側は stdin を1バイトずつ read する実装なので、入力は bytes でOK
        proc = subprocess.run(
            ["./a.out", str(w), str(h), str(r)],
            input=keystrokes.encode(),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )
    except FileNotFoundError:
        print("!! ./a.out が見つかりません。先に `cc life.c` などでビルドしてから実行してください。")
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
    - 盤外チェックは呼び出し側で気をつけて
    - toggle_off=True で最後に 'x' でもう一度トグルして描画OFF
    """
    keys = []
    cur_i, cur_j = start_i, start_j
    # 描画モードON
    keys.append('x')
    for (ti, tj) in cells:
        mv, cur_i, cur_j = path_moves(cur_i, cur_j, ti, tj)
        keys.append(mv)
        # 今の実装は「移動した位置が即描画される」挙動（描画ON時）なので、
        # 目的座標に到達すれば 0 が置かれる想定。
        # その場で「点打ち」したい場合は、同じマスで tiny move→戻るなどで対応可能。
    if toggle_off:
        keys.append('x')
    return "".join(keys)

if __name__ == "__main__":
    # 1) 質問で使っていたケース
    run_case(
        "case: 長い入力列 / 10x6 / 0世代",
        10, 6, 0,
        "sdxssdswdxddddsxaadwxwdxwaa"
    )

    # 2) ブリンカー（縦→横へ変化）: 3x3 / 1世代
    #   縦の3セル: (0,1), (1,1), (2,1) を描きたい
    blinker_cells = [(0,1), (1,1), (2,1)]
    keys_blinker = draw_cells(blinker_cells, 3, 3, start_i=0, start_j=0, toggle_off=True)
    run_case("blinker 3x3 / r=1", 3, 3, 1, keys_blinker)

    # 3) 手打ちの短い例（あなたが試した 'dxss'）
    run_case("dxss 3x3 / r=1", 3, 3, 1, "dxss")

    # 4) グライダー（5x5 / 4世代）を座標指定で描く（左上起点）
    #   典型的な初期配置の一つ（向きによっては微修正してください）
    glider = [(0,1), (1,2), (2,0), (2,1), (2,2)]
    keys_glider = draw_cells(glider, 5, 5, toggle_off=True)
    run_case("glider 5x5 / r=4", 5, 5, 4, keys_glider)


