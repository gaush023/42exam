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
    sleep 0.2
}

# Start server
start_server() {
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

# Test 1: Basic POST command
echo "Test Group 1: POST Operations"
start_server
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
