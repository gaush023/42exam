#!/bin/bash

EXEC="./mini_serv"
PORT=5556

# Colors
GREEN="\033[32m"
RED="\033[31m"
BLUE="\033[34m"
YELLOW="\033[33m"
RESET="\033[0m"

PASSED=0
FAILED=0

print_ok() {
    echo -e "  ${GREEN}✓${RESET} $1"
    ((PASSED++))
}

print_fail() {
    echo -e "  ${RED}✗${RESET} $1"
    ((FAILED++))
}

start_server() {
    $EXEC $PORT > server_log.txt 2>&1 &
    SERVER_PID=$!
    sleep 0.3
}

stop_server() {
    kill $SERVER_PID 2>/dev/null
    wait $SERVER_PID 2>/dev/null
}

cleanup() {
    rm -f c*.txt test_*.txt server_log.txt
    pkill -f "nc.*$PORT" 2>/dev/null
    stop_server 2>/dev/null
}

get_id() {
    # arrival から client ID を取り出す
    local file="$1"
    local id=$(grep "server: client " "$file" | head -n 1 | sed -E 's/.*client ([0-9]+).*/\1/')
    echo "$id"
}

echo -e "${BLUE}=========================================="
echo -e "  mini_serv Hard Tester (macOS version)"
echo -e "==========================================${RESET}"

############################################################
# Test 0
############################################################
echo -e "\n${BLUE}Test 0: Wrong number of arguments${RESET}"

OUT=$(./mini_serv 2>&1)
[[ "$OUT" == "Wrong number of arguments" ]] \
    && print_ok "Wrong number of arguments" \
    || print_fail "Expected error"

OUT=$(./mini_serv 1 2 2>&1)
[[ "$OUT" == "Wrong number of arguments" ]] \
    && print_ok "Too many arguments" \
    || print_fail "Expected error"

############################################################
# Test 1: Multiple clients
############################################################
echo -e "\n${BLUE}Test 1: Multiple clients (5 clients)${RESET}"

start_server

for i in {0..4}; do
    (sleep 0.6) | nc 127.0.0.1 $PORT > c${i}.txt &
    sleep 0.25
done
sleep 1.0

ARR0=$(grep -c "just arrived" c0.txt)
[[ "$ARR0" -eq 4 ]] \
    && print_ok "Client 0 received 4 arrivals" \
    || print_fail "Client 0 saw $ARR0 (expected 4)"

ARR2=$(grep -c "just arrived" c2.txt)
[[ "$ARR2" -eq 2 ]] \
    && print_ok "Client 2 saw 2 arrivals" \
    || print_fail "Client 2 saw $ARR2 (expected 2)"

cleanup

############################################################
# Test 2: Sequential IDs
############################################################
echo -e "\n${BLUE}Test 2: Sequential IDs${RESET}"

start_server

(sleep 0.5) | nc 127.0.0.1 $PORT > c0.txt &
sleep 0.3
(sleep 0.5) | nc 127.0.0.1 $PORT > c1.txt &
sleep 0.3
(sleep 0.5) | nc 127.0.0.1 $PORT > c2.txt &
sleep 0.8

ID0=$(get_id c0.txt)
ID1=$((ID0 + 1))
ID2=$((ID0 + 2))

grep -q "client $ID1 just arrived" c0.txt && \
grep -q "client $ID2 just arrived" c0.txt \
    && print_ok "IDs sequential ($ID0,$ID1,$ID2)" \
    || print_fail "IDs incorrect (got: $(grep 'just arrived' c0.txt))"

cleanup

############################################################
# Test 3: Multi-line
############################################################
echo -e "\n${BLUE}Test 3: Multi-line message${RESET}"

start_server

(sleep 0.5) | nc 127.0.0.1 $PORT > c0.txt &
sleep 0.3

(echo -e "Hello\nWorld"; sleep 0.6) | nc 127.0.0.1 $PORT > c1.txt &
sleep 1

ID0=$(get_id c0.txt)
ID1=$((ID0 + 1))

grep -q "client $ID1: Hello" c0.txt && \
grep -q "client $ID1: World" c0.txt \
    && print_ok "Multi-line OK" \
    || print_fail "Multi-line FAIL"

cleanup

############################################################
# Test 5: multiple messages
############################################################
echo -e "\n${BLUE}Test 5: Multiple messages${RESET}"

start_server

(sleep 0.5) | nc 127.0.0.1 $PORT > c0.txt &
sleep 0.3

(echo -e "First\nSecond\nThird"; sleep 0.6) | nc 127.0.0.1 $PORT > c1.txt &
sleep 1

ID0=$(get_id c0.txt)
ID1=$((ID0 + 1))

CNT=$(grep -c "client $ID1:" c0.txt)

[[ "$CNT" -eq 3 ]] \
    && print_ok "3 messages received" \
    || print_fail "Got $CNT messages (expected 3)"

cleanup

############################################################
# Summary
############################################################
echo -e "\n${BLUE}=========================================="
echo -e "  Test Summary"
echo -e "==========================================${RESET}"
echo -e "${GREEN}Passed: $PASSED${RESET}"
echo -e "${RED}Failed: $FAILED${RESET}"
