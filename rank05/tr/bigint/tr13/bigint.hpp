#pragma once

#include <cctype>
#include <climits>
#include <iostream>
#include <vector>

class bigint {
    typedef int digit_t;
private:
    std::vector<digit_t> a;  // 下位桁が a[0]
    void f_n(unsigned int v);
    bool is_zero() const;
    void normalize();
    static unsigned int t_u(const bigint &s);

public:
    bigint() : a(1, 0) {}
    bigint(unsigned int v) { f_n(v); }
    bigint(const bigint &s) : a(s.a) {}

    std::string tostring() const;

    bigint operator+(const bigint &s) const;
    bigint &operator+=(const bigint &s);

    // 前置/後置インクリメント
    bigint &operator++();     // ++x
    bigint operator++(int);   // x++

    // digit shift（10進の桁シフト）
    bigint operator<<(unsigned int v) const;
    bigint operator>>(unsigned int v) const;

    bigint &operator<<=(unsigned int v) { *this = *this << v; return *this; }
    bigint &operator>>=(unsigned int v) { *this = *this >> v; return *this; }

    bigint operator<<(const bigint &s) const { return *this << t_u(s); }
    bigint operator>>(const bigint &s) const { return *this >> t_u(s); }

    bigint &operator<<=(const bigint &s) { return *this <<= t_u(s); }
    bigint &operator>>=(const bigint &s) { return *this >>= t_u(s); }

    // 比較
    bool operator==(const bigint &s) const { return a == s.a; }
    bool operator!=(const bigint &s) const { return !(*this == s); }
    bool operator<(const bigint &s) const;
    bool operator>(const bigint &s) const { return s < *this; }
    bool operator<=(const bigint &s) const { return *this == s || *this < s; }
    bool operator>=(const bigint &s) const { return *this == s || s < *this; }

    std::string getStr() const { return tostring(); }

    friend std::ostream &operator<<(std::ostream &os, const bigint &s);
};
