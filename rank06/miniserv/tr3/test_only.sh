#!/bin/bash

EXEC="./mini_serv"
PORT=5556

start_server(){
    $EXEC $PORT > server_log.txt 2>&1 &
    SERVER_PID=$!
    sleep 0.2
}

connect_hold(){
    socat - TCP:127.0.0.1:${PORT} > "$1" &
    echo $!
}

cleanup(){
    rm -f c*.txt server_log.txt
    pkill -f "socat.*${PORT}" 2>/dev/null
    kill $SERVER_PID 2>/dev/null
}

# Test 1
start_server

P0=$(connect_hold c0.txt); sleep 0.2
P1=$(connect_hold c1.txt); sleep 0.2
P2=$(connect_hold c2.txt); sleep 0.2
P3=$(connect_hold c3.txt); sleep 0.2
P4=$(connect_hold c4.txt); sleep 0.5

ARR0=$(grep -c "just arrived" c0.txt)
ARR2=$(grep -c "just arrived" c2.txt)

echo "C0: $ARR0 (expected 4)"
echo "C2: $ARR2 (expected 2)"

kill $P0 $P1 $P2 $P3 $P4 2>/dev/null
cleanup
