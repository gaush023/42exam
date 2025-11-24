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

stop_server

printf '\nTests complete.\n'

