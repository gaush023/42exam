#!/bin/bash

EXEC="./mini_db"
PORT=4242
SAVE="save_test.txt"

GREEN='\033[32m'
RED='\033[31m'
RESET='\033[0m'

PASS=0
FAIL=0

ok() {
    echo -e "${GREEN}  ✓ $1${RESET}"
    ((PASS++))
}

ng() {
    echo -e "${RED}  ✗ $1${RESET}"
    echo "    Expected: [$2]"
    echo "    Got:      [$3]"
    ((FAIL++))
}

cleanup() {
    # Use SIGINT instead of SIGKILL to allow proper shutdown
    pkill -INT -f "mini_db $PORT" 2>/dev/null
    sleep 0.3
    # Force kill if still running
    pkill -9 -f "mini_db $PORT" 2>/dev/null
    rm -f /tmp/nc_out_*.txt
}

test_cmd() {
    local cmd="$1"
    local expected="$2"
    local desc="$3"
    local outfile="/tmp/nc_out_$$.txt"

    echo -e "$cmd" | nc localhost "$PORT" | tr -d '\r\n' > "$outfile" 2>&1
    local got=$(cat "$outfile")
    rm -f "$outfile"

    if [ "$expected" = "$got" ]; then
        ok "$desc"
    else
        ng "$desc" "$expected" "$got"
    fi
}

start_server() {
    cleanup
    $EXEC $PORT $SAVE > /tmp/server_start.log 2>&1 &
    SERVER_PID=$!

    # Wait for server to be ready
    i=0
    while [ $i -lt 50 ]; do
        if grep -q "ready" /tmp/server_start.log 2>/dev/null; then
            return 0
        fi
        if ! kill -0 $SERVER_PID 2>/dev/null; then
            echo "Server failed to start"
            cat /tmp/server_start.log
            exit 1
        fi
        sleep 0.1
        i=$((i + 1))
    done

    echo "Timeout waiting for server to start"
    kill $SERVER_PID 2>/dev/null
    exit 1
}

stop_server() {
    if [ -n "$SERVER_PID" ]; then
        kill -INT $SERVER_PID 2>/dev/null
        wait $SERVER_PID 2>/dev/null
        # Give it a moment to finish writing to disk
        sleep 0.2
    fi
}

echo "======================================"
echo "Mini-DB Strict Test Suite"
echo "======================================"

# Test 1: Basic Operations
echo ""
echo "Test Suite 1: Basic Operations"
echo "--------------------------------------"
start_server

test_cmd "POST k1 v1" "0" "POST k1 v1"
test_cmd "POST k2 v2" "0" "POST k2 v2"
test_cmd "GET k1" "0 v1" "GET k1"
test_cmd "GET k2" "0 v2" "GET k2"
test_cmd "GET notfound" "1" "GET missing key"
test_cmd "DELETE k1" "0" "DELETE k1"
test_cmd "GET k1" "1" "GET k1 after delete"
test_cmd "DELETE notfound" "1" "DELETE non-existent key"

stop_server

# Test 2: Invalid Commands
echo ""
echo "Test Suite 2: Invalid Commands"
echo "--------------------------------------"
start_server

test_cmd "INVALID key" "2" "Invalid command"
test_cmd "POST key_only" "2" "POST without value"
test_cmd "GET key extra" "2" "GET with extra argument"
test_cmd "DELETE key extra" "2" "DELETE with extra argument"
test_cmd "post k1 v1" "2" "Lowercase command (post)"
test_cmd "get k1" "2" "Lowercase command (get)"
test_cmd "delete k1" "2" "Lowercase command (delete)"

stop_server

# Test 3: Edge Cases
echo ""
echo "Test Suite 3: Edge Cases"
echo "--------------------------------------"
start_server

test_cmd "POST k1 v1" "0" "POST k1"
test_cmd "POST k1 v2" "0" "POST k1 overwrite"
test_cmd "GET k1" "0 v2" "GET k1 after overwrite"
test_cmd "POST key_underscore value_underscore" "0" "POST with underscores"
test_cmd "GET key_underscore" "0 value_underscore" "GET with underscores"
test_cmd "POST 123 456" "0" "POST numeric key/value"
test_cmd "GET 123" "0 456" "GET numeric key"
test_cmd "DELETE 123" "0" "DELETE numeric key"

stop_server

# Test 4: Persistence
echo ""
echo "Test Suite 4: Persistence (SIGINT)"
echo "--------------------------------------"

# Clean start for persistence test
pkill -INT -f "mini_db $PORT" 2>/dev/null
sleep 0.3
pkill -9 -f "mini_db $PORT" 2>/dev/null
rm -f "$SAVE" /tmp/nc_out_*.txt

# Start fresh server
$EXEC $PORT $SAVE > /tmp/server_start.log 2>&1 &
SERVER_PID=$!
i=0
while [ $i -lt 50 ]; do
    if grep -q "ready" /tmp/server_start.log 2>/dev/null; then
        break
    fi
    if ! kill -0 $SERVER_PID 2>/dev/null; then
        echo "Server failed to start"
        exit 1
    fi
    sleep 0.1
    i=$((i + 1))
done

test_cmd "POST persist1 value1" "0" "POST persist1"
test_cmd "POST persist2 value2" "0" "POST persist2"
test_cmd "POST persist3 value3" "0" "POST persist3"

stop_server

# Restart server WITHOUT cleanup (to test persistence)
$EXEC $PORT $SAVE > /tmp/server_start.log 2>&1 &
SERVER_PID=$!
i=0
while [ $i -lt 50 ]; do
    if grep -q "ready" /tmp/server_start.log 2>/dev/null; then
        break
    fi
    if ! kill -0 $SERVER_PID 2>/dev/null; then
        echo "Server failed to start"
        exit 1
    fi
    sleep 0.1
    i=$((i + 1))
done

test_cmd "GET persist1" "0 value1" "GET persist1 after restart"
test_cmd "GET persist2" "0 value2" "GET persist2 after restart"
test_cmd "GET persist3" "0 value3" "GET persist3 after restart"

test_cmd "POST persist1 updated" "0" "UPDATE persist1"
test_cmd "DELETE persist2" "0" "DELETE persist2"

stop_server

# Second restart WITHOUT cleanup
$EXEC $PORT $SAVE > /tmp/server_start.log 2>&1 &
SERVER_PID=$!
i=0
while [ $i -lt 50 ]; do
    if grep -q "ready" /tmp/server_start.log 2>/dev/null; then
        break
    fi
    if ! kill -0 $SERVER_PID 2>/dev/null; then
        echo "Server failed to start"
        exit 1
    fi
    sleep 0.1
    i=$((i + 1))
done

test_cmd "GET persist1" "0 updated" "GET persist1 after 2nd restart"
test_cmd "GET persist2" "1" "GET persist2 after delete+restart"
test_cmd "GET persist3" "0 value3" "GET persist3 after 2nd restart"

stop_server

# Test 5: Concurrent Operations
echo ""
echo "Test Suite 5: Concurrent Connections"
echo "--------------------------------------"
start_server

# Parallel writes
(printf "POST c1 v1\n" | nc localhost $PORT > /tmp/nc_out_c1.txt 2>&1) &
PID1=$!
(printf "POST c2 v2\n" | nc localhost $PORT > /tmp/nc_out_c2.txt 2>&1) &
PID2=$!
(printf "POST c3 v3\n" | nc localhost $PORT > /tmp/nc_out_c3.txt 2>&1) &
PID3=$!
(printf "POST c4 v4\n" | nc localhost $PORT > /tmp/nc_out_c4.txt 2>&1) &
PID4=$!
(printf "POST c5 v5\n" | nc localhost $PORT > /tmp/nc_out_c5.txt 2>&1) &
PID5=$!

wait $PID1 $PID2 $PID3 $PID4 $PID5

sleep 0.5

test_cmd "GET c1" "0 v1" "Concurrent POST c1"
test_cmd "GET c2" "0 v2" "Concurrent POST c2"
test_cmd "GET c3" "0 v3" "Concurrent POST c3"
test_cmd "GET c4" "0 v4" "Concurrent POST c4"
test_cmd "GET c5" "0 v5" "Concurrent POST c5"

stop_server

# Summary
echo ""
echo "======================================"
echo "Test Results Summary"
echo "======================================"
echo -e "${GREEN}Passed: $PASS${RESET}"
echo -e "${RED}Failed: $FAIL${RESET}"
echo "Total:  $((PASS + FAIL))"
echo "======================================"

cleanup

if [ $FAIL -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${RESET}"
    exit 0
else
    echo -e "${RED}Some tests failed!${RESET}"
    exit 1
fi
