#!/bin/bash

PORT=5558
EXEC="./mini_serv"

echo "=== Test 2: Client IDs ==="
$EXEC $PORT > /dev/null 2>&1 &
SERVER_PID=$!
sleep 0.3

(sleep 2) | nc -w 3 127.0.0.1 $PORT > c0.txt &
C0=$!
sleep 0.2
(sleep 2) | nc -w 3 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 0.2
(sleep 2) | nc -w 3 127.0.0.1 $PORT > c2.txt &
C2=$!
sleep 0.5

kill $C0 $C1 $C2 2>/dev/null
wait 2>/dev/null

echo "c0.txt contents:"
cat c0.txt
echo ""
echo "c1.txt contents:"
cat c1.txt
echo ""

kill $SERVER_PID 2>/dev/null
rm -f c0.txt c1.txt c2.txt

echo ""
echo "=== Test 3: Multiple lines ==="
$EXEC $PORT > /dev/null 2>&1 &
SERVER_PID=$!
sleep 0.3

(sleep 2) | nc -w 3 127.0.0.1 $PORT > c0.txt &
C0=$!
sleep 0.3

(echo -e "Hello\nWorld"; sleep 1) | nc -w 3 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 1.5

kill $C0 $C1 2>/dev/null
wait 2>/dev/null

echo "c0.txt contents:"
cat c0.txt
echo ""
echo "c0.txt with visible characters:"
cat c0.txt | cat -A
echo ""

kill $SERVER_PID 2>/dev/null
rm -f c0.txt c1.txt

echo ""
echo "=== Test 9: Message format ==="
$EXEC $PORT > /dev/null 2>&1 &
SERVER_PID=$!
sleep 0.3

(sleep 2) | nc -w 3 127.0.0.1 $PORT > c0.txt &
C0=$!
sleep 0.3

(echo "TestMsg"; sleep 1) | nc -w 3 127.0.0.1 $PORT > c1.txt &
C1=$!
sleep 1.5

kill $C0 $C1 2>/dev/null
wait 2>/dev/null

echo "c0.txt contents:"
cat c0.txt
echo ""
echo "Looking for exact match of 'client 1: TestMsg':"
grep "^client 1: TestMsg$" c0.txt && echo "FOUND" || echo "NOT FOUND"
echo "With cat -A:"
cat c0.txt | cat -A

kill $SERVER_PID 2>/dev/null
rm -f c0.txt c1.txt
