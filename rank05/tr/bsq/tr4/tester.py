import random
import subprocess

def generate_map(width, height, empty='.', obstacle='o', full='B', prob=0.2):
    """ランダムマップを生成 (先頭行にメタ情報を追加)"""
    header = f"{height} {empty} {obstacle} {full}"
    lines = []
    for _ in range(height):
        row = ''.join(obstacle if random.random() < prob else empty for _ in range(width))
        lines.append(row)
    return [header] + lines

def write_map(filename, lines):
    with open(filename, "w") as f:
        for line in lines:
            f.write(line + "\n")

def run_bsq(binary="./a.out", mapfile="map.txt"):
    with open(mapfile, "r") as f:
        result = subprocess.run([binary], stdin=f, capture_output=True, text=True)
    return result.stdout

if __name__ == "__main__":
    W, H = 20, 10
    lines = generate_map(W, H, prob=0.2)
    write_map("map.txt", lines)

    print("=== Input Map ===")
    print("\n".join(lines))
    print("\n=== BSQ Output ===")
    print(run_bsq())

