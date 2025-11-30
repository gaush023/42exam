#!/bin/bash

EXEC="./mini_serv"
PORT=5556

GREEN="\033[32m"
RED="\033[31m"
BLUE="\033[34m"
RESET="\033[0m"
PASSED=0
FAILED=0

ok() {
    echo -e "  ${GREEN}✓${RESET} $1"
    PASSED=$((PASSED + 1))
}

fail() {
    echo -e "  ${RED}✗${RESET} $1"
    FAILED=$((FAILED + 1))
}

cleanup() {
    pkill -f "mini_serv $PORT" 2>/dev/null
    pkill -f "nc.*$PORT" 2>/dev/null
    rm -f /tmp/ms_*.txt /tmp/server_*.log 2>/dev/null
}

# クライアント接続用の関数（ncを使用、stdinをsleepで保持）
connect_client() {
    local output_file="$1"
    (sleep 100 | nc 127.0.0.1 $PORT > "$output_file" 2>&1) &
    echo $!
}

# メッセージ送信用の関数
send_message() {
    local message="$1"
    printf "%s" "$message" | nc 127.0.0.1 $PORT >/dev/null 2>&1
}

echo -e "${BLUE}=========================================="
echo -e " mini_serv New Tester"
echo -e "==========================================${RESET}"

cleanup

############################################################
# Test 0: Argument validation
############################################################
OUT=$($EXEC 2>&1)
if [[ "$OUT" == "Wrong number of arguments" ]]; then
    ok "No arguments error"
else
    fail "No arguments: got '$OUT'"
fi

OUT=$($EXEC 1 2 3 2>&1)
if [[ "$OUT" == "Wrong number of arguments" ]]; then
    ok "Too many arguments error"
else
    fail "Too many args: got '$OUT'"
fi

############################################################
# Test 1: Multiple client arrivals
############################################################
cleanup
$EXEC $PORT >/tmp/server_test1.log 2>&1 &
SRV=$!
sleep 0.3

# 5つのクライアントを接続
C0=$(connect_client /tmp/ms_c0.txt); sleep 0.3
C1=$(connect_client /tmp/ms_c1.txt); sleep 0.3
C2=$(connect_client /tmp/ms_c2.txt); sleep 0.3
C3=$(connect_client /tmp/ms_c3.txt); sleep 0.3
C4=$(connect_client /tmp/ms_c4.txt); sleep 0.5

# クライアント0は4つの通知を受け取るはず（1,2,3,4が到着）
ARR0=$(grep -c "just arrived" /tmp/ms_c0.txt 2>/dev/null || echo 0)
if [[ "$ARR0" -eq 4 ]]; then
    ok "Client 0: received 4 arrivals"
else
    fail "Client 0: got $ARR0 arrivals (expected 4)"
fi

# クライアント2は2つの通知を受け取るはず（3,4が到着）
ARR2=$(grep -c "just arrived" /tmp/ms_c2.txt 2>/dev/null || echo 0)
if [[ "$ARR2" -eq 2 ]]; then
    ok "Client 2: received 2 arrivals"
else
    fail "Client 2: got $ARR2 arrivals (expected 2)"
fi

kill $C0 $C1 $C2 $C3 $C4 $SRV 2>/dev/null
wait $C0 $C1 $C2 $C3 $C4 $SRV 2>/dev/null
sleep 0.3

############################################################
# Test 2: Sequential IDs
############################################################
cleanup
$EXEC $PORT >/tmp/server_test2.log 2>&1 &
SRV=$!
sleep 0.3

C0=$(connect_client /tmp/ms_id0.txt); sleep 0.3
C1=$(connect_client /tmp/ms_id1.txt); sleep 0.3
C2=$(connect_client /tmp/ms_id2.txt); sleep 0.5

# クライアント0が受け取った最初のIDを確認
FIRST_LINE=$(grep "server: client" /tmp/ms_id0.txt 2>/dev/null | head -n1)
if [[ "$FIRST_LINE" =~ client\ ([0-9]+) ]]; then
    ID0=${BASH_REMATCH[1]}
    ID1=$((ID0 + 1))
    ID2=$((ID0 + 2))

    # ID1とID2が順番に通知されているか確認
    if grep -q "client $ID1 just arrived" /tmp/ms_id0.txt && \
       grep -q "client $ID2 just arrived" /tmp/ms_id0.txt; then
        ok "Sequential IDs: $ID0, $ID1, $ID2"
    else
        fail "IDs not sequential"
    fi
else
    fail "Could not detect client IDs"
fi

kill $C0 $C1 $C2 $SRV 2>/dev/null
wait $C0 $C1 $C2 $SRV 2>/dev/null
sleep 0.3

############################################################
# Test 3: Message forwarding
############################################################
cleanup
$EXEC $PORT >/tmp/server_test3.log 2>&1 &
SRV=$!
sleep 0.3

C0=$(connect_client /tmp/ms_msg0.txt); sleep 0.3
C1=$(connect_client /tmp/ms_msg1.txt); sleep 0.3

# クライアント1からメッセージ送信
send_message "Hello\n"
sleep 0.3
send_message "World\n"
sleep 0.5

# クライアント0がメッセージを受け取ったか確認
if grep -q "Hello" /tmp/ms_msg0.txt && grep -q "World" /tmp/ms_msg0.txt; then
    ok "Message forwarding: multi-line"
else
    fail "Messages not forwarded correctly"
fi

kill $C0 $C1 $SRV 2>/dev/null
wait $C0 $C1 $SRV 2>/dev/null
sleep 0.3

############################################################
# Test 4: Multiple messages
############################################################
cleanup
$EXEC $PORT >/tmp/server_test4.log 2>&1 &
SRV=$!
sleep 0.3

C0=$(connect_client /tmp/ms_multi0.txt); sleep 0.3
C1=$(connect_client /tmp/ms_multi1.txt); sleep 0.3

# 複数メッセージを送信
send_message "First\n"
sleep 0.2
send_message "Second\n"
sleep 0.2
send_message "Third\n"
sleep 0.5

# クライアント0が3つのメッセージを受け取ったか確認
MSG_COUNT=$(grep -c "client" /tmp/ms_multi0.txt 2>/dev/null || echo 0)
if [[ "$MSG_COUNT" -ge 3 ]]; then
    ok "Multiple messages: received $MSG_COUNT"
else
    fail "Multiple messages: got $MSG_COUNT (expected >= 3)"
fi

kill $C0 $C1 $SRV 2>/dev/null
wait $C0 $C1 $SRV 2>/dev/null
sleep 0.3

############################################################
# Test 5: Client disconnect notification
############################################################
cleanup
$EXEC $PORT >/tmp/server_test5.log 2>&1 &
SRV=$!
sleep 0.3

C0=$(connect_client /tmp/ms_disc0.txt); sleep 0.3
C1=$(connect_client /tmp/ms_disc1.txt); sleep 0.3

# クライアント1を切断
kill $C1 2>/dev/null
sleep 0.5

# クライアント0が切断通知を受け取ったか確認
if grep -q "just left" /tmp/ms_disc0.txt; then
    ok "Disconnect notification received"
else
    fail "Disconnect notification not received"
fi

kill $C0 $SRV 2>/dev/null
wait $C0 $SRV 2>/dev/null

############################################################
# Cleanup and Summary
############################################################
cleanup

echo ""
echo -e "${GREEN}Passed: $PASSED${RESET}"
echo -e "${RED}Failed: $FAILED${RESET}"

if [[ $FAILED -eq 0 ]]; then
    echo -e "\n${GREEN}All tests passed!${RESET}"
    exit 0
else
    echo -e "\n${RED}Some tests failed.${RESET}"
    exit 1
fi
