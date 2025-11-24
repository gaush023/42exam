#!/bin/sh

EXEC="./mini_db"
PORT=4242
SAVE="save.txt"
SERVER_OUT="/tmp/server_log.txt"
CLIENT_OUT="/tmp/client_log.txt"

GREEN="$(printf '\033[32m')"
RED="$(printf '\033[31m')"
RESET="$(printf '\033[0m')"

ok() {
    printf '%s  ✓ %s%s\n' "$GREEN" "$1" "$RESET"
}

ng() {
    printf '%s  ✗ %s%s\n' "$RED" "$1" "$RESET"
}

run_server() {
    $EXEC $PORT $SAVE > "$SERVER_OUT" 2>&1 &
    PID=$!

    i=0
    while [ $i -lt 100 ]; do
        if grep -q "ready" "$SERVER_OUT"; then
            SERVER_PID=$PID
            return
        fi
        sleep 0.1
        i=$((i + 1))
    done

    ng "Server did not print ready"
    kill $PID 2>/dev/null
    exit 1
}

stop_server() {
    kill -INT "$SERVER_PID" 2>/dev/null
    wait "$SERVER_PID" 2>/dev/null
}

send_cmd() {
    printf '%s' "$1" | nc localhost "$PORT" > "$CLIENT_OUT" 2>&1
}

get_result() {
    tr -d '\r\n' < "$CLIENT_OUT"
}

expect() {
    exp="$1"
    got="$2"
    msg="$3"

    if [ "$exp" = "$got" ]; then
        ok "$msg"
    else
        ng "$msg"
        printf '    Expected: [%s]\n' "$exp"
        printf '    Got:      [%s]\n' "$got"
    fi
}

printf '%s\n' "--------------------------------------"
printf '%s\n' "Running Tests"
printf '%s\n' "--------------------------------------"

rm -f "$SAVE"

run_server

send_cmd "POST k1 v1\n"
expect "0" "$(get_result)" "POST k1 v1"

send_cmd "POST k2 v2\n"
expect "0" "$(get_result)" "POST k2 v2"

send_cmd "GET k1\n"
expect "0 v1" "$(get_result)" "GET k1"

send_cmd "GET k2\n"
expect "0 v2" "$(get_result)" "GET k2"

send_cmd "GET notfound\n"
expect "1" "$(get_result)" "GET missing"

send_cmd "DELETE k1\n"
expect "0" "$(get_result)" "DELETE k1"

send_cmd "GET k1\n"
expect "1" "$(get_result)" "GET k1 after delete"

send_cmd "HELLO aaa\n"
expect "2" "$(get_result)" "Invalid command"

# Edge case: POST without value
send_cmd "POST key_only\n"
expect "2" "$(get_result)" "POST without value (invalid)"

# Edge case: GET with extra argument
send_cmd "GET k2 extra\n"
expect "2" "$(get_result)" "GET with extra argument (invalid)"

# Edge case: DELETE with extra argument
send_cmd "DELETE k2 extra\n"
expect "2" "$(get_result)" "DELETE with extra argument (invalid)"

# Edge case: Empty command
send_cmd "\n"
expect "2" "$(get_result)" "Empty command"

# Overwrite existing key
send_cmd "POST k2 v2_updated\n"
expect "0" "$(get_result)" "POST overwrite k2"

send_cmd "GET k2\n"
expect "0 v2_updated" "$(get_result)" "GET k2 after overwrite"

# DELETE non-existent key
send_cmd "DELETE nonexistent\n"
expect "1" "$(get_result)" "DELETE non-existent key"

# Multiple operations in sequence
send_cmd "POST k3 v3\n"
expect "0" "$(get_result)" "POST k3 v3"

send_cmd "POST k4 v4\n"
expect "0" "$(get_result)" "POST k4 v4"

send_cmd "POST k5 v5\n"
expect "0" "$(get_result)" "POST k5 v5"

send_cmd "GET k3\n"
expect "0 v3" "$(get_result)" "GET k3"

send_cmd "GET k4\n"
expect "0 v4" "$(get_result)" "GET k4"

send_cmd "GET k5\n"
expect "0 v5" "$(get_result)" "GET k5"

# Case sensitivity test
send_cmd "post k6 v6\n"
expect "2" "$(get_result)" "lowercase post (invalid)"

send_cmd "get k2\n"
expect "2" "$(get_result)" "lowercase get (invalid)"

# Special characters in keys and values
send_cmd "POST key_with_underscore value_with_underscore\n"
expect "0" "$(get_result)" "POST with underscores"

send_cmd "GET key_with_underscore\n"
expect "0 value_with_underscore" "$(get_result)" "GET key with underscore"

# Numbers as keys and values
send_cmd "POST 123 456\n"
expect "0" "$(get_result)" "POST numeric key/value"

send_cmd "GET 123\n"
expect "0 456" "$(get_result)" "GET numeric key"

send_cmd "DELETE 123\n"
expect "0" "$(get_result)" "DELETE numeric key"

stop_server

printf '\n%s\n' "--------------------------------------"
printf '%s\n' "Testing Persistence (SIGINT)"
printf '%s\n' "--------------------------------------"

# Test persistence after SIGINT
run_server

send_cmd "POST persist1 value1\n"
expect "0" "$(get_result)" "POST persist1"

send_cmd "POST persist2 value2\n"
expect "0" "$(get_result)" "POST persist2"

send_cmd "POST persist3 value3\n"
expect "0" "$(get_result)" "POST persist3"

stop_server

# Restart server and check if data persists
run_server

send_cmd "GET persist1\n"
expect "0 value1" "$(get_result)" "GET persist1 after restart"

send_cmd "GET persist2\n"
expect "0 value2" "$(get_result)" "GET persist2 after restart"

send_cmd "GET persist3\n"
expect "0 value3" "$(get_result)" "GET persist3 after restart"

# Modify data and check persistence again
send_cmd "POST persist1 updated_value\n"
expect "0" "$(get_result)" "POST persist1 update"

send_cmd "DELETE persist2\n"
expect "0" "$(get_result)" "DELETE persist2"

stop_server

# Final restart to verify modifications persisted
run_server

send_cmd "GET persist1\n"
expect "0 updated_value" "$(get_result)" "GET persist1 after second restart"

send_cmd "GET persist2\n"
expect "1" "$(get_result)" "GET persist2 after delete and restart"

send_cmd "GET persist3\n"
expect "0 value3" "$(get_result)" "GET persist3 after second restart"

stop_server

printf '\n%s\n' "--------------------------------------"
printf '%s\n' "Testing Concurrent Connections"
printf '%s\n' "--------------------------------------"

rm -f "$SAVE"
run_server

# Send multiple commands in parallel (using separate output files)
OUT1="/tmp/client_out1.txt"
OUT2="/tmp/client_out2.txt"
OUT3="/tmp/client_out3.txt"

(printf 'POST concurrent1 val1\n' | nc localhost "$PORT" > "$OUT1" 2>&1) &
PID1=$!
(printf 'POST concurrent2 val2\n' | nc localhost "$PORT" > "$OUT2" 2>&1) &
PID2=$!
(printf 'POST concurrent3 val3\n' | nc localhost "$PORT" > "$OUT3" 2>&1) &
PID3=$!

wait $PID1 $PID2 $PID3

# Verify all concurrent writes succeeded
send_cmd "GET concurrent1\n"
res1=$(get_result)
send_cmd "GET concurrent2\n"
res2=$(get_result)
send_cmd "GET concurrent3\n"
res3=$(get_result)

expect "0 val1" "$res1" "Concurrent POST concurrent1"
expect "0 val2" "$res2" "Concurrent POST concurrent2"
expect "0 val3" "$res3" "Concurrent POST concurrent3"

rm -f "$OUT1" "$OUT2" "$OUT3"

stop_server

printf '\n%s\n' "--------------------------------------"
printf '%s\n' "All Tests Complete"
printf '%s\n' "--------------------------------------"

