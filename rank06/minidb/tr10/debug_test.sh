#!/bin/bash

rm -f save.txt
./mini_db 4242 save.txt 2>&1 | head -30 &
SERVER_PID=$!

sleep 2

echo "=== Testing POST ==="
printf 'POST k1 v1\n' | nc localhost 4242

sleep 1

echo "=== Testing GET ==="
printf 'GET k1\n' | nc localhost 4242

sleep 1

kill -INT $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null
