#!/bin/bash

# Strict tester for mini_serv
EXEC="./mini_serv"
PORT=5558

GREEN="\033[32m"
RED="\033[31m"
BLUE="\033[34m"
RESET="\033[0m"

PASSED=0
FAILED=0

print_ok() {
    echo -e "  ${GREEN}✓${RESET} $1"
    ((PASSED++)) || true
}

print_fail() {
    echo -e "  ${RED}✗${RESET} $1"
    ((FAILED++)) || true
}

start_server() {
    $EXEC $PORT > /dev/null 2>&1 &
    SERVER_PID=$!
    sleep 0.2
}

stop_server() {
    kill $SERVER_PID 2>/dev/null
    wait $SERVER_PID 2>/dev/null
    pkill -f "nc.*$PORT" 2>/dev/null
    rm -f /tmp/fifo* test_*.txt c*.txt 2>/dev/null
}

echo -e "${BLUE}========================================"
echo -e "  mini_serv Strict Tester"
echo -e "========================================${RESET}"

##############################################################################
# Test 1: Wrong number of arguments
##############################################################################
echo -e "\n${BLUE}Test 1: Wrong number of arguments${RESET}"

OUT=$(./mini_serv 2>&1)
[[ "$OUT" == "Wrong number of arguments" ]] && print_ok "No arguments" || print_fail "No arguments (got: $OUT)"

OUT=$(./mini_serv 1 2 3 2>&1)
[[ "$OUT" == "Wrong number of arguments" ]] && print_ok "Too many arguments" || print_fail "Too many arguments (got: $OUT)"

##############################################################################
# Test 2: Basic client connection
##############################################################################
echo -e "\n${BLUE}Test 2: Basic client connection${RESET}"

start_server

mkfifo /tmp/fifo1 /tmp/fifo2 2>/dev/null
cat /tmp/fifo1 | nc 127.0.0.1 $PORT > test_c1.txt &
C1=$!
sleep 0.3
cat /tmp/fifo2 | nc 127.0.0.1 $PORT > test_c2.txt &
C2=$!
sleep 0.3

echo "" > /tmp/fifo1 &
echo "" > /tmp/fifo2 &
sleep 0.4

grep -q "server: client 1 just arrived" test_c1.txt && print_ok "Client arrival notification" || print_fail "Client arrival notification (got: $(cat test_c1.txt))"

stop_server

##############################################################################
# Test 3: Sequential client IDs
##############################################################################
echo -e "\n${BLUE}Test 3: Sequential client IDs${RESET}"

start_server

mkfifo /tmp/fifo1 /tmp/fifo2 /tmp/fifo3 2>/dev/null
cat /tmp/fifo1 | nc 127.0.0.1 $PORT > test_c1.txt &
C1=$!
sleep 0.2
cat /tmp/fifo2 | nc 127.0.0.1 $PORT > test_c2.txt &
C2=$!
sleep 0.2
cat /tmp/fifo3 | nc 127.0.0.1 $PORT > test_c3.txt &
C3=$!
sleep 0.3

echo "" > /tmp/fifo1 &
echo "" > /tmp/fifo2 &
echo "" > /tmp/fifo3 &
sleep 0.4

if grep -q "client 1 just arrived" test_c1.txt && grep -q "client 2 just arrived" test_c1.txt; then
    print_ok "Client IDs are sequential"
else
    print_fail "Client IDs not sequential (got: $(cat test_c1.txt))"
fi

stop_server

##############################################################################
# Test 4: Message broadcast
##############################################################################
echo -e "\n${BLUE}Test 4: Message broadcast${RESET}"

start_server

mkfifo /tmp/fifo1 /tmp/fifo2 2>/dev/null
cat /tmp/fifo1 | nc 127.0.0.1 $PORT > test_c1.txt &
C1=$!
sleep 0.3
cat /tmp/fifo2 | nc 127.0.0.1 $PORT > test_c2.txt &
C2=$!
sleep 0.3

echo "Hello" > /tmp/fifo2 &
sleep 0.4

grep -q "client 1: Hello" test_c1.txt && print_ok "Message broadcast" || print_fail "Message broadcast (got: $(cat test_c1.txt))"

echo "" > /tmp/fifo1 &
echo "" > /tmp/fifo2 &
sleep 0.3

stop_server

##############################################################################
# Test 5: Multiple messages
##############################################################################
echo -e "\n${BLUE}Test 5: Multiple messages${RESET}"

start_server

mkfifo /tmp/fifo1 /tmp/fifo2 2>/dev/null
cat /tmp/fifo1 | nc 127.0.0.1 $PORT > test_c1.txt &
C1=$!
sleep 0.3
cat /tmp/fifo2 | nc 127.0.0.1 $PORT > test_c2.txt &
C2=$!
sleep 0.3

printf "First\nSecond\nThird\n" > /tmp/fifo2 &
sleep 0.5

COUNT=$(grep -c "client 1:" test_c1.txt)
[ "$COUNT" -eq 3 ] && print_ok "Multiple messages (got $COUNT)" || print_fail "Multiple messages (got $COUNT, expected 3)"

echo "" > /tmp/fifo1 &
echo "" > /tmp/fifo2 &
sleep 0.3

stop_server

##############################################################################
# Test 6: Disconnect notification
##############################################################################
echo -e "\n${BLUE}Test 6: Disconnect notification${RESET}"

start_server

mkfifo /tmp/fifo1 /tmp/fifo2 2>/dev/null
cat /tmp/fifo1 | nc 127.0.0.1 $PORT > test_c1.txt &
C1=$!
sleep 0.3
cat /tmp/fifo2 | nc 127.0.0.1 $PORT > test_c2.txt &
C2=$!
sleep 0.3

# Disconnect client 2
echo "" > /tmp/fifo2 &
sleep 0.4

grep -q "server: client 1 just left" test_c1.txt && print_ok "Disconnect notification" || print_fail "Disconnect notification (got: $(cat test_c1.txt))"

echo "" > /tmp/fifo1 &
sleep 0.3

stop_server

##############################################################################
# Test 7: Message without newline (should not be sent)
##############################################################################
echo -e "\n${BLUE}Test 7: Message without newline${RESET}"

start_server

mkfifo /tmp/fifo1 /tmp/fifo2 2>/dev/null
cat /tmp/fifo1 | nc 127.0.0.1 $PORT > test_c1.txt &
C1=$!
sleep 0.3
cat /tmp/fifo2 | nc 127.0.0.1 $PORT > test_c2.txt &
C2=$!
sleep 0.3

printf "NoNewline" > /tmp/fifo2 &
sleep 0.4

! grep -q "client 1: NoNewline" test_c1.txt && print_ok "No-newline not sent" || print_fail "No-newline was sent"

echo "" > /tmp/fifo1 &
echo "" > /tmp/fifo2 &
sleep 0.3

stop_server

##############################################################################
# Test 8: Large message
##############################################################################
echo -e "\n${BLUE}Test 8: Large message (1000 chars)${RESET}"

start_server

mkfifo /tmp/fifo1 /tmp/fifo2 2>/dev/null
cat /tmp/fifo1 | nc 127.0.0.1 $PORT > test_c1.txt &
C1=$!
sleep 0.3
cat /tmp/fifo2 | nc 127.0.0.1 $PORT > test_c2.txt &
C2=$!
sleep 0.3

python3 -c "print('A' * 1000)" > /tmp/fifo2 &
sleep 0.5

grep -q "AAAAAAAAAA" test_c1.txt && print_ok "Large message sent" || print_fail "Large message failed"

echo "" > /tmp/fifo1 &
echo "" > /tmp/fifo2 &
sleep 0.3

stop_server

##############################################################################
# Test 9: Empty message
##############################################################################
echo -e "\n${BLUE}Test 9: Empty message${RESET}"

start_server

mkfifo /tmp/fifo1 /tmp/fifo2 2>/dev/null
cat /tmp/fifo1 | nc 127.0.0.1 $PORT > test_c1.txt &
C1=$!
sleep 0.3
cat /tmp/fifo2 | nc 127.0.0.1 $PORT > test_c2.txt &
C2=$!
sleep 0.3

echo "" > /tmp/fifo2 &
sleep 0.4

grep -E "^client 1: $" test_c1.txt > /dev/null && print_ok "Empty message sent" || print_fail "Empty message not sent correctly"

echo "" > /tmp/fifo1 &
sleep 0.3

stop_server

##############################################################################
# Test 10: Concurrent messages from multiple clients
##############################################################################
echo -e "\n${BLUE}Test 10: Concurrent messages${RESET}"

start_server

mkfifo /tmp/fifo0 /tmp/fifo1 /tmp/fifo2 2>/dev/null
cat /tmp/fifo0 | nc 127.0.0.1 $PORT > test_c0.txt &
C0=$!
sleep 0.3
cat /tmp/fifo1 | nc 127.0.0.1 $PORT > test_c1.txt &
C1=$!
sleep 0.2
cat /tmp/fifo2 | nc 127.0.0.1 $PORT > test_c2.txt &
C2=$!
sleep 0.3

echo "From1" > /tmp/fifo1 &
sleep 0.1
echo "From2" > /tmp/fifo2 &
sleep 0.5

if grep -q "client 1: From1" test_c0.txt && grep -q "client 2: From2" test_c0.txt; then
    print_ok "Concurrent messages received"
else
    print_fail "Concurrent messages failed (got: $(cat test_c0.txt))"
fi

echo "" > /tmp/fifo0 &
echo "" > /tmp/fifo1 &
echo "" > /tmp/fifo2 &
sleep 0.3

stop_server

##############################################################################
# Summary
##############################################################################
echo -e "\n${BLUE}========================================"
echo -e "  Test Summary"
echo -e "========================================${RESET}"
echo -e "${GREEN}Passed: $PASSED${RESET}"
echo -e "${RED}Failed: $FAILED${RESET}"

rm -f /tmp/fifo* test_*.txt c*.txt 2>/dev/null

if [ $FAILED -eq 0 ]; then
    echo -e "\n${GREEN}All tests passed!${RESET}"
    exit 0
else
    echo -e "\n${RED}Some tests failed.${RESET}"
    exit 1
fi
