#!/bin/bash

# Path to your program
EXEC="./mini_serv"
PORT=5555

# Colors
GREEN="\033[32m"
RED="\033[31m"
BLUE="\033[34m"
RESET="\033[0m"

function print_ok() {
    echo -e "  ${GREEN}✓${RESET} $1"
}

function print_fail() {
    echo -e "  ${RED}✗${RESET} $1"
}

function start_server() {
    $EXEC $PORT > server_log.txt 2>&1 &
    SERVER_PID=$!
    sleep 0.2
}

function stop_server() {
    kill $SERVER_PID 2>/dev/null
    wait $SERVER_PID 2>/dev/null
}

echo -e "${BLUE}=========================================="
echo -e "  mini_serv Tester"
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

############################################################
# Test 1: Server startup
############################################################
echo -e "\n${BLUE}Test 1: Server startup${RESET}"

start_server
sleep 0.2
if kill -0 $SERVER_PID 2>/dev/null; then
    print_ok "Server started"
else
    print_fail "Server failed to start"
fi
stop_server

############################################################
# Test 2: Client arrival + broadcast
############################################################
echo -e "\n${BLUE}Test 2: Arrival broadcast${RESET}"

start_server

# Client 1 connects first
(sleep 2) | nc -w 3 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 0.3

# Client 2 connects - Client 1 should receive notification
(sleep 2) | nc -w 3 127.0.0.1 $PORT > c2.txt &
C2=$!
sleep 0.5

wait $C1 $C2 2>/dev/null

ARRIVAL=$(cat c1.txt)
EXPECTED="server: client 1 just arrived"

if echo "$ARRIVAL" | grep -q "$EXPECTED"; then
    print_ok "Broadcast arrival OK"
else
    print_fail "Broadcast arrival FAIL (got: $ARRIVAL)"
fi

stop_server

############################################################
# Test 3: Message broadcast
############################################################
echo -e "\n${BLUE}Test 3: Message broadcast${RESET}"

start_server

# Client 1 - will receive messages
(sleep 2) | nc -w 3 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 0.3

# Client 2 - will send a message
(sleep 0.2; echo -e "Hello"; sleep 1) | nc -w 3 127.0.0.1 $PORT > c2.txt &
C2=$!
sleep 1

wait $C1 $C2 2>/dev/null

MSG=$(cat c1.txt | grep "client 1:")

if echo "$MSG" | grep -q "client 1: Hello"; then
    print_ok "Broadcast message OK"
else
    print_fail "Broadcast message FAIL (got: $(cat c1.txt))"
fi

stop_server

############################################################
# Test 4: Disconnect broadcast
############################################################
echo -e "\n${BLUE}Test 4: Disconnect${RESET}"

start_server

# Client 1 - will receive disconnect notification (stays connected longer)
(sleep 3) | nc -w 4 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 0.3

# Client 2 - will disconnect early
(sleep 0.5) | nc -w 1 127.0.0.1 $PORT > c2.txt &
C2=$!
sleep 0.3

# Wait for client 2 to disconnect
wait $C2 2>/dev/null
sleep 0.3

# Now kill client 1 to get output
kill $C1 2>/dev/null
wait $C1 2>/dev/null

LD=$(cat c1.txt | grep "just left")

if echo "$LD" | grep -q "server: client 1 just left"; then
    print_ok "Disconnect broadcast OK"
else
    print_fail "Disconnect broadcast FAIL (got: $(cat c1.txt))"
fi

stop_server

echo -e "\n${GREEN}All tests completed.${RESET}"

