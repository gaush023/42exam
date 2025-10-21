import subprocess

# 今の main 出力に合わせて期待値を変更
test_cases = [
    {
        "args": [5, 10, 15],
        "expected": ["5 10 15", "1", "0"]
    },
    {
        "args": [1],
        "expected": ["1", "0"]
    },
    {
        "args": [1, 2, 3, 4, 5],
        "expected": ["1 2 3 4 5"]
    },
    {
        "args": [10, 10, 20],
        "expected": ["10", "20"]
    },
]

def run_test(args, expected_keywords):
    cmd = ["./a.out"] + list(map(str, args))
    print(f"\n=== Running: {' '.join(cmd)} ===")
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        output = result.stdout.strip()
        print(output)

        # 判定（部分一致）
        passed = True
        for keyword in expected_keywords:
            if keyword not in output:
                print(f"❌ Missing expected: {keyword}")
                passed = False
        if passed:
            print("✅ Test Passed")
        else:
            print("❌ Test Failed")

    except subprocess.CalledProcessError as e:
        print(f"Error running {cmd}: {e}")
        print(e.stdout)
        print(e.stderr)


if __name__ == "__main__":
    for case in test_cases:
        run_test(case["args"], case["expected"])
