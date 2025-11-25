#!/bin/bash

send_cmd() {
    printf '%s' "$1" | nc localhost 4242 > /tmp/test_out.txt 2>&1
}

rm -f save.txt
./mini_db 4242 save.txt 2>/tmp/server_debug.log &
SERVER_PID=$!

sleep 2

echo "=== Testing with tester's method ==="
send_cmd "POST k1 v1\n"
echo "POST result: $(cat /tmp/test_out.txt | tr -d '\r\n')"

sleep 1

send_cmd "GET k1\n"
echo "GET result: $(cat /tmp/test_out.txt | tr -d '\r\n')"

sleep 1

echo ""
echo "=== Server debug log ==="
cat /tmp/server_debug.log

kill -INT $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null
