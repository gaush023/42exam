#include "bigint.hpp"

void bigint::f_n(unsigned int v) {
    a.clear();
    if (v == 0) { a.push_back(0); return; }
    while (v > 0) {
        a.push_back(v % 10);
        v /= 10;
    }
}

std::string bigint::tostring() const {
    int i = (int)a.size() - 1;
    while (i > 0 && a[i] == 0) i--;
    std::string s;
    s.reserve(i + 1);
    for (; i >= 0; --i) {
        s.push_back(char(a[i] + '0'));
    }
    return s;
}

std::ostream &operator<<(std::ostream &os, const bigint &s) {
    os << s.tostring();
    return os;
}

bigint bigint::operator+(const bigint &s) const {
    bigint tmp = *this;
    tmp += s;
    return tmp;
}

bigint &bigint::operator+=(const bigint &s) {
    size_t l = std::max(a.size(), s.a.size());
    a.resize(l, 0);
    int carry = 0;
    for (size_t i = 0; i < l; i++) {
        int sum = a[i] + (i < s.a.size() ? s.a[i] : 0) + carry;
        a[i] = sum % 10;
        carry = sum / 10;
    }
    while (carry > 0) {
        a.push_back(carry % 10);
        carry /= 10;
    }
    normalize();
    return *this;
}

bigint &bigint::operator++() {  // 前置
    *this += bigint(1);
    return *this;
}

bigint bigint::operator++(int) {  // 後置
    bigint tmp = *this;
    ++(*this);
    return tmp;
}

bool bigint::is_zero() const {
    return a.size() == 1 && a[0] == 0;
}

bigint bigint::operator<<(unsigned int n) const {
    if (is_zero()) return *this;
    bigint out = *this;
    // 上位桁側（末尾）にゼロを追加 → ×10^n
    out.a.insert(out.a.end(), n, 0);
    return out;
}

bigint bigint::operator>>(unsigned int n) const {
    if (n == 0) return *this;
    if (n >= a.size()) return bigint(0u);
    bigint out = *this;
    // 下位桁を削除 → ÷10^n
    out.a.erase(out.a.begin(), out.a.begin() + n);
    out.normalize();
    return out;
}

void bigint::normalize() {
    while (a.size() > 1 && a.back() == 0) a.pop_back();
    if (a.empty()) a.push_back(0);
}

unsigned int bigint::t_u(const bigint &s) {
    unsigned int v = 0;
    for (int i = (int)s.a.size() - 1; i >= 0; --i) {
        v = v * 10 + s.a[i];
        if (v > UINT_MAX) return UINT_MAX;
    }
    return v;
}

bool bigint::operator<(const bigint &s) const {
    if (a.size() != s.a.size()) return a.size() < s.a.size();
    for (int i = (int)a.size() - 1; i >= 0; --i) {
        if (a[i] != s.a[i]) return a[i] < s.a[i];
    }
    return false;
}
