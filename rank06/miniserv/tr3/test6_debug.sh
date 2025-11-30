#!/bin/bash

PORT=5558
rm -f /tmp/fifo1 /tmp/fifo2 test_c1.txt test_c2.txt

# Start server
./mini_serv $PORT > /dev/null 2>&1 &
SERVER_PID=$!
sleep 0.3

# Create fifos
mkfifo /tmp/fifo1 /tmp/fifo2

# Connect client 1
cat /tmp/fifo1 | nc 127.0.0.1 $PORT > test_c1.txt &
C1=$!
echo "Client 1 PID: $C1"
sleep 0.3

# Connect client 2
cat /tmp/fifo2 | nc 127.0.0.1 $PORT > test_c2.txt &
C2=$!
echo "Client 2 PID: $C2"
sleep 0.3

echo "=== After both clients connected ==="
cat test_c1.txt
echo "---"

# Disconnect client 2
echo "Sending empty line to client 2..."
echo "" > /tmp/fifo2 &
sleep 0.4

echo "=== After sending empty line (0.4s) ==="
cat test_c1.txt
echo "---"

sleep 0.5
echo "=== After 0.5s more ==="
cat test_c1.txt
echo "---"

sleep 1
echo "=== After 1s more ==="
cat test_c1.txt
echo "---"

# Check if client 2 is still running
if kill -0 $C2 2>/dev/null; then
    echo "Client 2 is still running"
else
    echo "Client 2 has terminated"
fi

# Cleanup
echo "" > /tmp/fifo1 &
sleep 0.3

kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

rm -f /tmp/fifo1 /tmp/fifo2
