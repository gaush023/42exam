#!/bin/bash

rm -f save.txt
./mini_db 4242 save.txt &
SERVER_PID=$!

sleep 2

echo "=== Testing POST k1 v1 ==="
printf 'POST k1 v1\n' | nc localhost 4242 | cat -A
echo ""

echo "=== Testing GET k1 ==="
printf 'GET k1\n' | nc localhost 4242 | cat -A
echo ""

kill -INT $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null
