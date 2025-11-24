#!/bin/bash

print_ok() {
    echo "  ✓ $1"
}

print_fail() {
    echo "  ✗ $1"
}

echo "Test 1: Wrong number of arguments"

OUT=$(./mini_serv 2>&1)
[[ "$OUT" == "Wrong number of arguments" ]] && print_ok "No arguments" || print_fail "No arguments (got: $OUT)"

OUT=$(./mini_serv 1 2 3 2>&1)
[[ "$OUT" == "Wrong number of arguments" ]] && print_ok "Too many arguments" || print_fail "Too many arguments (got: $OUT)"
