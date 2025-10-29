#include "bigint.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace
{
    std::size_t g_total = 0;
    std::size_t g_failed = 0;

    void report_failure(const std::string &name, const std::string &message)
    {
        std::cerr << "[FAIL] " << name << ": " << message << '\n';
        ++g_failed;
    }

    void check(bool condition, const std::string &name, const std::string &message)
    {
        ++g_total;
        if (!condition)
        {
            report_failure(name, message);
        }
    }

    void check_equal(const bigint &actual, const std::string &expected,
                     const std::string &name)
    {
        check(actual.getStr() == expected, name,
              "expected " + expected + ", got " + actual.getStr());
    }

    void check_bool(bool actual, bool expected, const std::string &name)
    {
        check(actual == expected, name,
              std::string("expected ") + (expected ? "true" : "false") +
                  ", got " + (actual ? "true" : "false"));
    }
}

int main()
{
    // Constructors and streaming
    {
        bigint zero;
        check_equal(zero, "0", "default-constructor-zero");
        check_equal(bigint(0), "0", "value-constructor-zero");
        check_equal(bigint(123456789u), "123456789", "value-constructor-positive");

        std::ostringstream oss;
        oss << bigint(42);
        check(oss.str() == "42", "stream-operator", "stream output mismatch");
    }

    // Addition and carry propagation
    {
        check_equal(bigint(0) + bigint(0), "0", "addition-zero-zero");
        check_equal(bigint(1) + bigint(0), "1", "addition-one-zero");
        check_equal(bigint(999999999u) + bigint(1), "1000000000", "addition-carry-overflow");

        bigint lhs(123456789u);
        bigint rhs(987654321u);
        check_equal(lhs + rhs, "1111111110", "addition-long");

        bigint accum(0);
        for (int i = 0; i < 10; ++i)
        {
            accum += bigint(111111111u);
        }
        check_equal(accum, "1111111110", "addition-compound-assignment");
    }

    // Increment operators
    {
        bigint value(41);
        bigint pre = ++value;
        check_equal(value, "42", "pre-increment-value");
        check_equal(pre, "42", "pre-increment-return");

        bigint post = value++;
        check_equal(value, "43", "post-increment-value");
        check_equal(post, "42", "post-increment-return");
    }

    // Digit shifts with unsigned values
    {
        bigint number(42);
        check_equal(number << 0, "42", "shift-left-zero");
        check_equal(number << 1, "420", "shift-left-one");
        check_equal(number << 5, "4200000", "shift-left-five");

        bigint large = bigint(1) << 25;
        check_equal(large, "10000000000000000000000000", "shift-left-large");

        check_equal(number >> 0, "42", "shift-right-zero");
        check_equal(number >> 1, "4", "shift-right-one");
        check_equal(number >> 2, "0", "shift-right-all-digits");
        check_equal(large >> 30, "0", "shift-right-beyond-size");

        bigint chained(12345);
        chained <<= 3;
        check_equal(chained, "12345000", "shift-left-assign");
        chained >>= 5;
        check_equal(chained, "123", "shift-right-assign");
    }

    // Digit shifts with bigint operands
    {
        bigint number(1337);
        bigint shift_amount(2);

        check_equal(number << shift_amount, "133700", "shift-left-bigint");
        check_equal(number >> shift_amount, "13", "shift-right-bigint");

        bigint lhs(9000);
        lhs <<= bigint(3);
        check_equal(lhs, "9000000", "shift-left-assign-bigint");
        lhs >>= bigint(6);
        check_equal(lhs, "9", "shift-right-assign-bigint");
    }

    // Comparisons
    {
        bigint a(12345);
        bigint b(12345);
        bigint c(54321);
        bigint d(12346);

        check_bool(a == b, true, "comparison-equal");
        check_bool(a != b, false, "comparison-not-equal");
        check_bool(a < c, true, "comparison-less-than");
        check_bool(c > a, true, "comparison-greater-than");
        check_bool(a <= b, true, "comparison-less-equal");
        check_bool(c >= d, true, "comparison-greater-equal");
        check_bool(a > a, false, "comparison-greater-than-self");
        check_bool(a >= c, false, "comparison-greater-equal-false");
    }

    // Large value manipulations
    {
        // 1 << 10 = 1 の後ろに 0 が10個（= 10^10 = 10000000000）
        bigint big = bigint(1) << 10;
        bigint addend(99999);
        bigint sum = big + addend;

        // 修正: 10^10 + 99999 = 10,000,099,999
        check_equal(sum, "10000099999", "large-value-addition");

        bigint copy(sum);
        check_equal(copy, "10000099999", "copy-constructor");

        copy += bigint(1);
        // 10,000,099,999 + 1 = 10,000,100,000
        check_equal(copy, "10000100000", "copy-after-addition");
    }

    if (g_failed != 0)
    {
        std::cerr << g_failed << " / " << g_total << " tests failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "All tests passed (" << g_total << ")" << std::endl;
    return EXIT_SUCCESS;
}
