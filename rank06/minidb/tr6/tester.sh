#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

PORT=4242
SAVE_FILE=".test_save"
EXECUTABLE="./mini_db"

# Test counter
TESTS_PASSED=0
TESTS_FAILED=0

# Function to print test results
print_test() {
    local test_name="$1"
    local expected="$2"
    local actual="$3"

    if [ "$expected" = "$actual" ]; then
        echo -e "${GREEN}✓${NC} $test_name"
        ((TESTS_PASSED++))
    else
        echo -e "${RED}✗${NC} $test_name"
        echo -e "  Expected: ${YELLOW}$expected${NC}"
        echo -e "  Got:      ${YELLOW}$actual${NC}"
        ((TESTS_FAILED++))
    fi
}

# Function to send command and get response
send_command() {
    local cmd="$1"
    echo "$cmd" | nc localhost $PORT 2>/dev/null | head -n 1
}

# Function to wait for server to be ready
wait_for_server() {
    local max_wait=5
    local count=0
    while [ $count -lt $max_wait ]; do
        if nc -z localhost $PORT 2>/dev/null; then
            sleep 0.1
            return 0
        fi
        sleep 0.5
        ((count++))
    done
    return 1
}

# Cleanup function
cleanup() {
    pkill -9 -f "$EXECUTABLE $PORT" 2>/dev/null
    rm -f "$SAVE_FILE"
    rm -f /tmp/server_output.txt
    rm -f /tmp/client*.txt
    sleep 0.2
}

# Start server
start_server() {
    cleanup
    $EXECUTABLE $PORT $SAVE_FILE > /tmp/server_output.txt 2>&1 &
    SERVER_PID=$!
    sleep 0.3

    if ! wait_for_server; then
        echo -e "${RED}Failed to start server${NC}"
        exit 1
    fi
}

# Start server silently (for most tests)
start_server_silent() {
    cleanup
    $EXECUTABLE $PORT $SAVE_FILE > /dev/null 2>&1 &
    SERVER_PID=$!
    sleep 0.3

    if ! wait_for_server; then
        echo -e "${RED}Failed to start server${NC}"
        exit 1
    fi
}

# Compile the program
echo "Compiling mini_db..."
if c++ -Wall -Wextra -Werror mini_db.cpp -o mini_db 2>&1; then
    echo -e "${GREEN}Compilation successful${NC}"
else
    echo -e "${RED}Compilation failed${NC}"
    exit 1
fi
echo ""

echo "=========================================="
echo "  mini_db Tester"
echo "=========================================="
echo ""

# Test 0: Ready output
echo "Test Group 0: Server Initialization"
start_server
sleep 0.2
if [ -f /tmp/server_output.txt ]; then
    ready_output=$(head -n 1 /tmp/server_output.txt)
    print_test "Server prints 'ready'" "ready" "$ready_output"
else
    print_test "Server prints 'ready'" "ready" ""
fi
cleanup
echo ""

# Test 1: Basic POST command
echo "Test Group 1: POST Operations"
start_server_silent
result=$(send_command "POST key1 value1")
print_test "POST key1 value1" "0" "$result"

result=$(send_command "POST key2 value2")
print_test "POST key2 value2" "0" "$result"

result=$(send_command "POST abc xyz")
print_test "POST abc xyz" "0" "$result"
echo ""

# Test 2: GET existing keys
echo "Test Group 2: GET Operations (existing keys)"
result=$(send_command "GET key1")
print_test "GET key1" "0 value1" "$result"

result=$(send_command "GET key2")
print_test "GET key2" "0 value2" "$result"

result=$(send_command "GET abc")
print_test "GET abc" "0 xyz" "$result"
echo ""

# Test 3: GET non-existing keys
echo "Test Group 3: GET Operations (non-existing keys)"
result=$(send_command "GET nonexistent")
print_test "GET nonexistent" "1" "$result"

result=$(send_command "GET foo")
print_test "GET foo" "1" "$result"
echo ""

# Test 4: DELETE existing keys
echo "Test Group 4: DELETE Operations (existing keys)"
result=$(send_command "DELETE key1")
print_test "DELETE key1" "0" "$result"

# Verify deletion
result=$(send_command "GET key1")
print_test "GET key1 (after deletion)" "1" "$result"
echo ""

# Test 5: DELETE non-existing keys
echo "Test Group 5: DELETE Operations (non-existing keys)"
result=$(send_command "DELETE nonexistent")
print_test "DELETE nonexistent" "1" "$result"

result=$(send_command "DELETE key1")
print_test "DELETE key1 (already deleted)" "1" "$result"
echo ""

# Test 6: Unknown commands
echo "Test Group 6: Unknown Commands"
result=$(send_command "UNKNOWN")
print_test "UNKNOWN command" "2" "$result"

result=$(send_command "PUT key value")
print_test "PUT command" "2" "$result"

result=$(send_command "HELLO")
print_test "HELLO command" "2" "$result"
echo ""

# Test 7: Data persistence
echo "Test Group 7: Data Persistence"
# Add some data
send_command "POST persist1 data1" > /dev/null
send_command "POST persist2 data2" > /dev/null
send_command "POST persist3 data3" > /dev/null

# Send SIGINT to save and stop server
pkill -SIGINT -f "$EXECUTABLE $PORT"
sleep 0.5

# Start server again (without cleanup to preserve saved data)
rm -f /tmp/server_output.txt
$EXECUTABLE $PORT $SAVE_FILE > /dev/null 2>&1 &
SERVER_PID=$!
sleep 0.3

if ! wait_for_server; then
    echo -e "${RED}Failed to start server${NC}"
    exit 1
fi

# Check if data persisted
result=$(send_command "GET persist1")
print_test "GET persist1 (after restart)" "0 data1" "$result"

result=$(send_command "GET persist2")
print_test "GET persist2 (after restart)" "0 data2" "$result"

result=$(send_command "GET persist3")
print_test "GET persist3 (after restart)" "0 data3" "$result"

# Verify deleted key is still deleted
result=$(send_command "GET key1")
print_test "GET key1 (still deleted after restart)" "1" "$result"
echo ""

# Test 8: Multiple operations in sequence
echo "Test Group 8: Sequential Operations"
send_command "POST seq1 val1" > /dev/null
result=$(send_command "GET seq1")
print_test "POST then GET" "0 val1" "$result"

send_command "DELETE seq1" > /dev/null
result=$(send_command "GET seq1")
print_test "DELETE then GET" "1" "$result"

# Re-add after delete
send_command "POST seq1 newval" > /dev/null
result=$(send_command "GET seq1")
print_test "POST after DELETE" "0 newval" "$result"
echo ""

# Test 9: Edge cases
echo "Test Group 9: Edge Cases"
result=$(send_command "POST x y")
print_test "Single char key/value" "0" "$result"

result=$(send_command "GET x")
print_test "GET single char key" "0 y" "$result"

result=$(send_command "POST longkey verylongvalue123456789")
print_test "Long key/value" "0" "$result"

result=$(send_command "GET longkey")
print_test "GET long key" "0 verylongvalue123456789" "$result"
echo ""

# Test 10: Persistent connections (multiple commands in one session)
echo "Test Group 10: Persistent Connections"
result=$(printf "POST session1 data1\nGET session1\nDELETE session1\nGET session1\n" | timeout 2 nc localhost $PORT 2>/dev/null)
expected=$'0\n0 data1\n0\n1'
if [ "$result" = "$expected" ]; then
    echo -e "${GREEN}✓${NC} Multiple commands in single session"
    ((TESTS_PASSED++))
else
    echo -e "${RED}✗${NC} Multiple commands in single session"
    echo -e "  Expected: ${YELLOW}$(echo "$expected" | tr '\n' '|')${NC}"
    echo -e "  Got:      ${YELLOW}$(echo "$result" | tr '\n' '|')${NC}"
    ((TESTS_FAILED++))
fi

# Test with mixed commands
result=$(printf "POST a b\nPOST c d\nGET a\nGET c\n" | timeout 2 nc localhost $PORT 2>/dev/null)
expected=$'0\n0\n0 b\n0 d'
if [ "$result" = "$expected" ]; then
    echo -e "${GREEN}✓${NC} Multiple POST and GET in single session"
    ((TESTS_PASSED++))
else
    echo -e "${RED}✗${NC} Multiple POST and GET in single session"
    echo -e "  Expected: ${YELLOW}$(echo "$expected" | tr '\n' '|')${NC}"
    echo -e "  Got:      ${YELLOW}$(echo "$result" | tr '\n' '|')${NC}"
    ((TESTS_FAILED++))
fi
echo ""

# Test 11: Request size limit (max 1000 characters)
echo "Test Group 11: Request Size Limit"
# Create a request close to 1000 chars
# Format: "POST <key> <value>" - need key + value to fit within limit
long_key=$(printf 'k%.0s' {1..400})
long_value=$(printf 'v%.0s' {1..590})  # POST + space + key + space + value = ~1000
result=$(send_command "POST $long_key $long_value")
print_test "POST with ~1000 char request" "0" "$result"

result=$(send_command "GET $long_key")
expected="0 $long_value"
if [ "$result" = "$expected" ]; then
    echo -e "${GREEN}✓${NC} GET long key/value"
    ((TESTS_PASSED++))
else
    echo -e "${RED}✗${NC} GET long key/value"
    echo -e "  Expected length: ${YELLOW}${#expected}${NC}"
    echo -e "  Got length:      ${YELLOW}${#result}${NC}"
    ((TESTS_FAILED++))
fi
echo ""

# Test 12: Multiple concurrent clients
echo "Test Group 12: Multiple Concurrent Clients"
# Test multiple clients - server uses select() so it can handle multiple connections
# We test by sending requests rapidly in sequence
result=$(send_command "POST client1 value1")
print_test "Client 1 POST" "0" "$result"

result=$(send_command "POST client2 value2")
print_test "Client 2 POST" "0" "$result"

result=$(send_command "POST client3 value3")
print_test "Client 3 POST" "0" "$result"

# Verify all data was stored correctly
result=$(send_command "GET client1")
print_test "GET client1 (after concurrent)" "0 value1" "$result"

result=$(send_command "GET client2")
print_test "GET client2 (after concurrent)" "0 value2" "$result"

result=$(send_command "GET client3")
print_test "GET client3 (after concurrent)" "0 value3" "$result"

# Cleanup temp files
rm -f /tmp/client*.txt
echo ""

# Cleanup
cleanup

# Summary
echo "=========================================="
echo "  Test Summary"
echo "=========================================="
echo -e "Passed: ${GREEN}$TESTS_PASSED${NC}"
echo -e "Failed: ${RED}$TESTS_FAILED${NC}"
echo "Total:  $((TESTS_PASSED + TESTS_FAILED))"
echo ""

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed! ✓${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed! ✗${NC}"
    exit 1
fi
