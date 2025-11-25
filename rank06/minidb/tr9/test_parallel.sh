#!/bin/bash

PORT=4242

# Start server
./mini_db $PORT .test_save > /dev/null 2>&1 &
sleep 1

echo "Starting parallel clients..."

# Run parallel clients
(echo "POST client1 value1" | nc localhost $PORT 2>/dev/null | head -n 1) > /tmp/client1.txt &
(echo "POST client2 value2" | nc localhost $PORT 2>/dev/null | head -n 1) > /tmp/client2.txt &
(echo "POST client3 value3" | nc localhost $PORT 2>/dev/null | head -n 1) > /tmp/client3.txt &

wait

echo "Results:"
cat /tmp/client1.txt
cat /tmp/client2.txt
cat /tmp/client3.txt

# Cleanup
pkill -9 mini_db
