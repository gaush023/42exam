#!/bin/bash

# Clean up
pkill -9 mini_serv 2>/dev/null
pkill -9 nc 2>/dev/null
rm -f /tmp/fifo1 /tmp/fifo2 test_manual_*.txt

# Start server
./mini_serv 5560 > manual_server.log 2>&1 &
SERVER_PID=$!
echo "Server PID: $SERVER_PID"
sleep 0.5

# Check if server is running
if ! kill -0 $SERVER_PID 2>/dev/null; then
    echo "ERROR: Server failed to start"
    cat manual_server.log
    exit 1
fi

# Create fifos
mkfifo /tmp/fifo1 /tmp/fifo2

# Connect client 1
cat /tmp/fifo1 | nc 127.0.0.1 5560 > test_manual_c1.txt &
C1=$!
echo "Client 1 PID: $C1"
sleep 0.5

# Connect client 2
cat /tmp/fifo2 | nc 127.0.0.1 5560 > test_manual_c2.txt &
C2=$!
echo "Client 2 PID: $C2"
sleep 0.5

# Check what client 1 received
echo "=== Client 1 should have received arrival notification ==="
cat test_manual_c1.txt

# Send message from client 2
echo "Hello from client 2" > /tmp/fifo2 &
sleep 0.5

# Check what client 1 received
echo "=== Client 1 should have received the message ==="
cat test_manual_c1.txt

# Close clients
echo "" > /tmp/fifo1 &
echo "" > /tmp/fifo2 &
sleep 0.5

# Check final output
echo "=== Final client 1 output ==="
cat test_manual_c1.txt

# Kill server
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

# Cleanup
rm -f /tmp/fifo1 /tmp/fifo2
