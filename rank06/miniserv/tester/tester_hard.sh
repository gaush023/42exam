#!/bin/bash

# Path to your program
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

function print_ok() {
    echo -e "  ${GREEN}✓${RESET} $1"
    ((PASSED++))
}

function print_fail() {
    echo -e "  ${RED}✗${RESET} $1"
    ((FAILED++))
}

function print_warn() {
    echo -e "  ${YELLOW}⚠${RESET} $1"
}

function start_server() {
    $EXEC $PORT > server_log.txt 2>&1 &
    SERVER_PID=$!
    sleep 0.3
}

function stop_server() {
    kill $SERVER_PID 2>/dev/null
    wait $SERVER_PID 2>/dev/null
}

function cleanup() {
    rm -f c*.txt test_*.txt
    pkill -f "nc.*$PORT" 2>/dev/null
    stop_server
}

echo -e "${BLUE}=========================================="
echo -e "  mini_serv Hard Tester"
echo -e "==========================================${RESET}"

############################################################
# Test 0: Wrong number of arguments
############################################################
echo -e "\n${BLUE}Test 0: Wrong number of arguments${RESET}"

OUT=$(./mini_serv 2>&1)
if [[ "$OUT" == "Wrong number of arguments" ]]; then
    print_ok "Wrong number of arguments"
else
    print_fail "Expected 'Wrong number of arguments'"
fi

OUT=$(./mini_serv 1 2 3 2>&1)
if [[ "$OUT" == "Wrong number of arguments" ]]; then
    print_ok "Too many arguments"
else
    print_fail "Expected 'Wrong number of arguments' for 3 args"
fi

############################################################
# Test 1: Multiple clients (5 clients)
############################################################
echo -e "\n${BLUE}Test 1: Multiple clients (5 clients)${RESET}"

start_server

# Start 5 clients
for i in {0..4}; do
    (sleep 0.8) | nc -w 1 127.0.0.1 $PORT > c${i}.txt &
    eval "C${i}=$!"
    sleep 0.2
done

sleep 0.2

# Wait for all to finish
wait 2>/dev/null
sleep 0.1

# Client 0 should see arrivals of clients 1, 2, 3, 4
ARRIVALS=$(cat c0.txt | grep "just arrived" | wc -l | tr -d ' ')
if [ "$ARRIVALS" -eq 4 ]; then
    print_ok "Client 0 received 4 arrival notifications"
else
    print_fail "Client 0 received $ARRIVALS arrivals, expected 4"
fi

# Client 2 should see arrivals of clients 3, 4
ARRIVALS=$(cat c2.txt | grep "just arrived" | wc -l | tr -d ' ')
if [ "$ARRIVALS" -eq 2 ]; then
    print_ok "Client 2 received 2 arrival notifications"
else
    print_fail "Client 2 received $ARRIVALS arrivals, expected 2"
fi

cleanup

############################################################
# Test 2: Client IDs are sequential
############################################################
echo -e "\n${BLUE}Test 2: Client IDs are sequential${RESET}"

start_server

# Connect 3 clients
(sleep 0.6) | nc -w 1 127.0.0.1 $PORT > c0.txt &
C0=$!
sleep 0.2
(sleep 0.6) | nc -w 1 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 0.2
(sleep 0.6) | nc -w 1 127.0.0.1 $PORT > c2.txt &
C2=$!
sleep 0.3

# Wait for connections to close
wait $C0 $C1 $C2 2>/dev/null
sleep 0.1

# Check if client 0 sees "client 1" and "client 2"
if grep -q "client 1 just arrived" c0.txt && grep -q "client 2 just arrived" c0.txt; then
    print_ok "Client IDs are sequential (0, 1, 2)"
else
    print_fail "Client IDs are not sequential (got: $(cat c0.txt | grep 'just arrived'))"
fi

cleanup

############################################################
# Test 3: Message with newline handling
############################################################
echo -e "\n${BLUE}Test 3: Message with newline handling${RESET}"

start_server

(sleep 0.7) | nc -w 1 127.0.0.1 $PORT > c0.txt &
C0=$!
sleep 0.4

(echo -e "Hello\nWorld"; sleep 0.8) | nc -w 1 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 1

wait $C0 $C1 2>/dev/null
sleep 0.2

# Client 0 should receive two separate messages
MSG1=$(grep "client 1: Hello" c0.txt)
MSG2=$(grep "client 1: World" c0.txt)

if [ -n "$MSG1" ] && [ -n "$MSG2" ]; then
    print_ok "Multiple lines handled correctly"
else
    print_fail "Multiple lines not handled correctly (got: $(cat c0.txt | grep 'client 1:'))"
fi

cleanup

############################################################
# Test 4: Message without trailing newline
############################################################
echo -e "\n${BLUE}Test 4: Message without trailing newline${RESET}"

start_server

(sleep 0.8) | nc -w 1 127.0.0.1 $PORT > c0.txt &
C0=$!
sleep 0.3

(printf "NoNewline"; sleep 0.5) | nc -w 1 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 1

kill $C0 $C1 2>/dev/null
wait 2>/dev/null

# Without newline, message should not be sent
if grep -q "client 1: NoNewline" c0.txt; then
    print_fail "Message without newline was sent (should not be)"
else
    print_ok "Message without newline not sent (correct)"
fi

cleanup

############################################################
# Test 5: Multiple messages from same client
############################################################
echo -e "\n${BLUE}Test 5: Multiple messages from same client${RESET}"

start_server

(sleep 0.8) | nc -w 1 127.0.0.1 $PORT > c0.txt &
C0=$!
sleep 0.4

(echo -e "First\nSecond\nThird"; sleep 1) | nc -w 1 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 0.7

wait $C0 $C1 2>/dev/null
sleep 0.2

# Client 0 should receive all three messages
COUNT=$(grep -c "client 1:" c0.txt)
if [ "$COUNT" -eq 3 ]; then
    print_ok "Received 3 messages from same client"
else
    print_fail "Received $COUNT messages, expected 3 (got: $(cat c0.txt))"
fi

cleanup

############################################################
# Test 6: Concurrent messages from multiple clients
############################################################
echo -e "\n${BLUE}Test 6: Concurrent messages from multiple clients${RESET}"

start_server

(sleep 0.8) | nc -w 1 127.0.0.1 $PORT > c0.txt &
C0=$!
sleep 0.4

(sleep 0.3; echo "From1"; sleep 0.8) | nc -w 1 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 0.2

(sleep 0.3; echo "From2"; sleep 0.8) | nc -w 1 127.0.0.1 $PORT > c2.txt &
C2=$!
sleep 0.8

wait $C0 $C1 $C2 2>/dev/null
sleep 0.2

# Client 0 should receive messages from both client 1 and client 2
if grep -q "client 1: From1" c0.txt && grep -q "client 2: From2" c0.txt; then
    print_ok "Received concurrent messages from multiple clients"
else
    print_fail "Did not receive concurrent messages correctly (got: $(cat c0.txt))"
fi

cleanup

############################################################
# Test 7: Disconnect notifications
############################################################
echo -e "\n${BLUE}Test 7: Multiple disconnect notifications${RESET}"

start_server

(sleep 1) | nc -w 4 127.0.0.1 $PORT > c0.txt &
C0=$!
sleep 0.3

(sleep 0.5) | nc -w 1 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 0.2

(sleep 0.7) | nc -w 1 127.0.0.1 $PORT > c2.txt &
C2=$!
sleep 0.8

kill $C0 2>/dev/null
wait 2>/dev/null

# Client 0 should see both disconnects
DISCONNECTS=$(grep -c "just left" c0.txt)
if [ "$DISCONNECTS" -eq 2 ]; then
    print_ok "Received 2 disconnect notifications"
else
    print_fail "Received $DISCONNECTS disconnects, expected 2"
fi

cleanup

############################################################
# Test 8: Large message (within buffer)
############################################################
echo -e "\n${BLUE}Test 8: Large message (within buffer)${RESET}"

start_server

(sleep 0.8) | nc -w 1 127.0.0.1 $PORT > c0.txt &
C0=$!
sleep 0.3

# Create a message with 1000 characters
LARGE_MSG=$(python3 -c "print('A' * 1000)")
(echo "$LARGE_MSG"; sleep 1) | nc -w 1 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 0.8

kill $C0 $C1 2>/dev/null
wait 2>/dev/null

if grep -q "AAAAAAAAAA" c0.txt; then
    print_ok "Large message transmitted successfully"
else
    print_fail "Large message not transmitted"
fi

cleanup

############################################################
# Test 9: Message format validation
############################################################
echo -e "\n${BLUE}Test 9: Message format validation${RESET}"

start_server

(sleep 0.7) | nc -w 1 127.0.0.1 $PORT > c0.txt &
C0=$!
sleep 0.4

(echo "TestMsg"; sleep 0.8) | nc -w 1 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 1

wait $C0 $C1 2>/dev/null
sleep 0.2

# Check format includes "client 1: TestMsg"
# Note: There might be arrival notification too
if grep -q "client 1: TestMsg" c0.txt; then
    print_ok "Message format is correct"
else
    print_fail "Message format is incorrect (got: $(cat c0.txt | grep 'client 1:'))"
fi

cleanup

############################################################
# Test 10: Rapid connect/disconnect
############################################################
echo -e "\n${BLUE}Test 10: Rapid connect/disconnect${RESET}"

start_server

(sleep 0.8) | nc -w 1 127.0.0.1 $PORT > test_main.txt &
CMAIN=$!
sleep 0.3

# Rapidly connect and disconnect 3 clients
for i in {1..3}; do
    (sleep 0.2) | nc -w 1 127.0.0.1 $PORT > /dev/null &
    sleep 0.3
done

sleep 1
kill $CMAIN 2>/dev/null
wait 2>/dev/null

# Should see 3 arrivals and 3 departures
ARRIVALS=$(grep -c "just arrived" test_main.txt)
DEPARTURES=$(grep -c "just left" test_main.txt)

if [ "$ARRIVALS" -eq 3 ] && [ "$DEPARTURES" -eq 3 ]; then
    print_ok "Rapid connect/disconnect handled correctly"
else
    print_fail "Arrivals: $ARRIVALS (expected 3), Departures: $DEPARTURES (expected 3)"
fi

cleanup

############################################################
# Test 11: Empty line handling
############################################################
echo -e "\n${BLUE}Test 11: Empty line handling${RESET}"

start_server

(sleep 0.7) | nc -w 1 127.0.0.1 $PORT > c0.txt &
C0=$!
sleep 0.4

(echo ""; sleep 0.8) | nc -w 1 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 1

wait $C0 $C1 2>/dev/null
sleep 0.2

# Empty line should be sent as empty message (just "client 1: \n")
# Look for "client 1: " followed by newline
if grep -E "^client 1: $" c0.txt > /dev/null; then
    print_ok "Empty line handled correctly"
else
    print_fail "Empty line not handled correctly (got: $(cat c0.txt | cat -A))"
fi

cleanup

############################################################
# Test 12: No fd_set corruption (server socket excluded)
############################################################
echo -e "\n${BLUE}Test 12: Server socket not treated as client${RESET}"

start_server

(sleep 0.7) | nc -w 1 127.0.0.1 $PORT > c0.txt &
C0=$!
sleep 0.4

(echo "Test"; sleep 0.6) | nc -w 1.5 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 0.9

wait $C0 $C1 2>/dev/null
sleep 0.2

# Should only see client 1's arrival and message, not any message from server socket
# Expected: "server: client 1 just arrived" and "client 1: Test"
ARRIVAL=$(grep -c "server: client 1 just arrived" c0.txt)
MSG=$(grep -c "client 1: Test" c0.txt)

if [ "$ARRIVAL" -eq 1 ] && [ "$MSG" -eq 1 ]; then
    print_ok "Server socket correctly excluded from clients"
else
    print_fail "Unexpected messages (arrival: $ARRIVAL, msg: $MSG, got: $(cat c0.txt))"
fi

cleanup

############################################################
# Summary
############################################################
echo -e "\n${BLUE}=========================================="
echo -e "  Test Summary"
echo -e "==========================================${RESET}"
echo -e "${GREEN}Passed: $PASSED${RESET}"
echo -e "${RED}Failed: $FAILED${RESET}"

if [ $FAILED -eq 0 ]; then
    echo -e "\n${GREEN}All tests passed! 🎉${RESET}"
    exit 0
else
    echo -e "\n${RED}Some tests failed.${RESET}"
    exit 1
fi
