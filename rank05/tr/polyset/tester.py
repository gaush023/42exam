import subprocess

# テストケース: 引数と期待される出力（部分一致でもOKにしてある）
test_cases = [
    {
        "args": [5, 10, 15],
        "expected": [
            "Tree bag",   # 部分一致チェック
            "Array bag",
            "Checking 5",
            "Checking 10",
            "Checking 15"
        ]
    },
    {
        "args": [1],
        "expected": [
            "Tree bag",
            "Array bag",
            "Checking 1"
        ]
    },
    {
        "args": [1, 2, 3, 4, 5],
        "expected": [
            "Tree bag",
            "Array bag",
            "Checking 5"
        ]
    },
    {
        "args": [10, 10, 20],  # 重複あり
        "expected": [
            "Tree bag",
            "Array bag",
            "Checking 10",
            "Checking 20"
        ]
    },
]

def run_test(args, expected_keywords):
    cmd = ["./a.out"] + list(map(str, args))
    print(f"\n=== Running: {' '.join(cmd)} ===")
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        output = result.stdout
        print(output)

        # 判定
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
        print(f"Error: {e}")
        print(e.stdout)
        print(e.stderr)


if __name__ == "__main__":
    for case in test_cases:
        run_test(case["args"], case["expected"])
