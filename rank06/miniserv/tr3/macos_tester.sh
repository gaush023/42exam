#!/bin/zsh

EXEC="./mini_serv"
PORT=5556

GREEN="\033[32m"; RED="\033[31m"; BLUE="\033[34m"; RESET="\033[0m"
PASSED=0; FAILED=0

ok(){ echo -e "  ${GREEN}✓${RESET} $1"; ((PASSED++)) || true; }
fail(){ echo -e "  ${RED}✗${RESET} $1"; ((FAILED++)) || true; }

start_server(){
    $EXEC $PORT > server_log.txt 2>&1 &
    SERVER_PID=$!
    sleep 0.5
    ps -p $SERVER_PID >/dev/null 2>&1 || echo "WARNING: Server not running!"
}

stop_server(){
    kill $SERVER_PID 2>/dev/null
    wait $SERVER_PID 2>/dev/null
}

cleanup(){
    rm -f c*.txt server_log.txt
    pkill -f "socat.*${PORT}" 2>/dev/null
    stop_server 2>/dev/null
}

# macOS: socatなら接続維持できる
connect_hold(){
    socat - TCP:127.0.0.1:${PORT} > "$1" &
    echo $!
}

send_msg(){
    printf "%s" "$2" | socat - TCP:127.0.0.1:${PORT}
}

echo -e "${BLUE}=========================================="
echo -e " mini_serv Tester (macOS socat stable)"
echo -e "==========================================${RESET}"

############################################################
# Test 0
############################################################
OUT=$(./mini_serv 2>&1)
[[ "$OUT" == "Wrong number of arguments" ]] && ok "Wrong number" || fail "Wrong number"

OUT=$(./mini_serv 1 2 2>&1)
[[ "$OUT" == "Wrong number of arguments" ]] && ok "Too many args" || fail "Too many args"

############################################################
# Test 1: Multiple clients
############################################################
start_server

P0=$(connect_hold c0.txt); sleep 0.2
P1=$(connect_hold c1.txt); sleep 0.2
P2=$(connect_hold c2.txt); sleep 0.2
P3=$(connect_hold c3.txt); sleep 0.2
P4=$(connect_hold c4.txt); sleep 1.0

ls -lh c*.txt >&2
ARR0=$(grep -c "just arrived" c0.txt)
[[ "$ARR0" -eq 4 ]] && ok "Client0 arrival OK" || fail "Client0 got $ARR0"

ARR2=$(grep -c "just arrived" c2.txt)
[[ "$ARR2" -eq 2 ]] && ok "Client2 arrival OK" || fail "Client2 got $ARR2"

kill $P0 $P1 $P2 $P3 $P4 2>/dev/null
cleanup

############################################################
# Test 2: Sequential IDs
############################################################
start_server

P0=$(connect_hold c0.txt); sleep 0.2
P1=$(connect_hold c1.txt); sleep 0.2
P2=$(connect_hold c2.txt); sleep 0.4

ID0=$(grep "server: client" c0.txt | head -n1 | sed -E 's/.*client ([0-9]+).*/\1/')
ID1=$((ID0+1))
ID2=$((ID0+2))

grep -q "client $ID1 just arrived" c0.txt &&
grep -q "client $ID2 just arrived" c0.txt \
    && ok "Sequential IDs OK" || fail "ID mismatch"

kill $P0 $P1 $P2 2>/dev/null
cleanup

############################################################
# Test 3: Multi-line message
############################################################
start_server

P0=$(connect_hold c0.txt); sleep 0.3
P1=$(connect_hold c1.txt); sleep 0.3

send_msg "$P1" "Hello\n"; sleep 0.1
send_msg "$P1" "World\n"; sleep 0.4

grep -q "Hello" c0.txt && grep -q "World" c0.txt \
    && ok "Multi-line OK" || fail "Multi-line FAIL"

kill $P0 $P1
cleanup

############################################################
# Test 5: Multiple messages
############################################################
start_server

P0=$(connect_hold c0.txt); sleep 0.3
P1=$(connect_hold c1.txt); sleep 0.3

send_msg "$P1" "First\n"
send_msg "$P1" "Second\n"
send_msg "$P1" "Third\n"
sleep 0.4

CNT=$(grep -c "client" c0.txt)
[[ "$CNT" -eq 3 ]] && ok "3 messages OK" || fail "Got $CNT messages"

kill $P0 $P1
cleanup

############################################################
# Summary
############################################################
echo ""
echo -e "${GREEN}Passed: $PASSED${RESET}"
echo -e "${RED}Failed: $FAILED${RESET}"
