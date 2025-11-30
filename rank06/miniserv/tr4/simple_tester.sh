#!/bin/bash

EXEC="./mini_serv"
PORT=5556
GREEN="\033[32m"
RED="\033[31m"
BLUE="\033[34m"
RESET="\033[0m"
PASSED=0
FAILED=0

ok() { echo -e "  ${GREEN}✓${RESET} $1"; PASSED=$((PASSED + 1)); }
fail() { echo -e "  ${RED}✗${RESET} $1"; FAILED=$((FAILED + 1)); }

cleanup() {
    pkill -f "mini_serv $PORT" 2>/dev/null
    killall nc 2>/dev/null
    rm -f /tmp/test_*.txt 2>/dev/null
}

echo -e "${BLUE}=========================================="
echo " mini_serv Simple Tester"
echo "==========================================${RESET}"

############################################################
# Test 0: Arguments
############################################################
OUT=$($EXEC 2>&1)
[[ "$OUT" == "Wrong number of arguments" ]] && ok "No args" || fail "No args"

OUT=$($EXEC 1 2 3 2>&1)
[[ "$OUT" == "Wrong number of arguments" ]] && ok "Too many args" || fail "Too many args"

############################################################
# Test 1: Client arrivals
############################################################
cleanup
$EXEC $PORT &
SRV_PID=$!
sleep 0.3

# Connect 5 clients using nc with longer timeout
(sleep 10 | nc 127.0.0.1 $PORT > /tmp/test_c0.txt 2>&1) &
C0=$!
sleep 0.3

(sleep 10 | nc 127.0.0.1 $PORT > /tmp/test_c1.txt 2>&1) &
C1=$!
sleep 0.3

(sleep 10 | nc 127.0.0.1 $PORT > /tmp/test_c2.txt 2>&1) &
C2=$!
sleep 0.3

(sleep 10 | nc 127.0.0.1 $PORT > /tmp/test_c3.txt 2>&1) &
C3=$!
sleep 0.3

(sleep 10 | nc 127.0.0.1 $PORT > /tmp/test_c4.txt 2>&1) &
C4=$!
sleep 0.5

# Check arrivals
ARR0=$(grep -c "just arrived" /tmp/test_c0.txt 2>/dev/null || echo 0)
[[ "$ARR0" -eq 4 ]] && ok "C0: 4 arrivals" || fail "C0: $ARR0 arrivals"

ARR2=$(grep -c "just arrived" /tmp/test_c2.txt 2>/dev/null || echo 0)
[[ "$ARR2" -eq 2 ]] && ok "C2: 2 arrivals" || fail "C2: $ARR2 arrivals"

# Cleanup test 1
kill $C0 $C1 $C2 $C3 $C4 $SRV_PID 2>/dev/null
sleep 0.5

############################################################
# Test 2: Message broadcast
############################################################
cleanup
$EXEC $PORT &
SRV_PID=$!
sleep 0.3

# Client 0 (receiver)
(sleep 10 | nc 127.0.0.1 $PORT > /tmp/test_msg0.txt 2>&1) &
R0=$!
sleep 0.3

# Client 1 (receiver)
(sleep 10 | nc 127.0.0.1 $PORT > /tmp/test_msg1.txt 2>&1) &
R1=$!
sleep 0.3

# Send messages
echo "Hello" | nc 127.0.0.1 $PORT &
sleep 0.3
echo "World" | nc 127.0.0.1 $PORT &
sleep 0.5

# Check if messages were received
if grep -q "Hello" /tmp/test_msg0.txt && grep -q "World" /tmp/test_msg0.txt; then
    ok "Message broadcast"
else
    fail "Message broadcast"
fi

# Cleanup test 2
kill $R0 $R1 $SRV_PID 2>/dev/null
sleep 0.5

############################################################
# Test 3: Client departure
############################################################
cleanup
$EXEC $PORT &
SRV_PID=$!
sleep 0.3

# Client 0 (stays)
(sleep 10 | nc 127.0.0.1 $PORT > /tmp/test_left0.txt 2>&1) &
L0=$!
sleep 0.3

# Client 1 (will disconnect)
(sleep 1 | nc 127.0.0.1 $PORT > /tmp/test_left1.txt 2>&1) &
L1=$!
sleep 2

# Check if client 0 received departure notice
if grep -q "just left" /tmp/test_left0.txt; then
    ok "Departure notification"
else
    fail "Departure notification"
fi

# Cleanup test 3
kill $L0 $SRV_PID 2>/dev/null
sleep 0.5

############################################################
# Summary
############################################################
cleanup

echo ""
echo -e "${GREEN}Passed: $PASSED${RESET}"
echo -e "${RED}Failed: $FAILED${RESET}"

[[ $FAILED -eq 0 ]] && echo -e "\n${GREEN}ALL TESTS PASSED!${RESET}" && exit 0
echo -e "\n${RED}SOME TESTS FAILED${RESET}" && exit 1
