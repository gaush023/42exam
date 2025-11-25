#!/bin/bash

# Simple FIFO test to understand the disconnection issue
PORT=5559
rm -f /tmp/test_fifo test_output.txt

# Start server
./mini_serv $PORT > /dev/null 2>&1 &
SERVER_PID=$!
sleep 0.3

# Create FIFO
mkfifo /tmp/test_fifo

# Connect client using FIFO - this should keep reading until EOF
cat /tmp/test_fifo | nc 127.0.0.1 $PORT > test_output.txt &
CAT_PID=$!
NC_PID=$(pgrep -P $CAT_PID)
echo "cat PID: $CAT_PID"
sleep 0.5

echo "=== Initial connection ==="
cat test_output.txt

# Send message and close FIFO writer
echo "Sending message to FIFO and closing writer..."
echo "test" > /tmp/test_fifo &
ECHO_PID=$!
wait $ECHO_PID 2>/dev/null
echo "Echo process ($ECHO_PID) finished"
sleep 0.5

echo "=== After sending message ==="
cat test_output.txt

# Check if cat is still running
if kill -0 $CAT_PID 2>/dev/null; then
    echo "cat process is STILL RUNNING (this is the problem!)"
    echo "Force closing FIFO..."
    # Force close by exec
    exec 3>/tmp/test_fifo
    exec 3>&-
    sleep 0.5
    if kill -0 $CAT_PID 2>/dev/null; then
        echo "cat STILL running even after exec close"
    else
        echo "cat terminated after exec close"
    fi
else
    echo "cat process has terminated"
fi

# Cleanup
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null
rm -f /tmp/test_fifo test_output.txt
