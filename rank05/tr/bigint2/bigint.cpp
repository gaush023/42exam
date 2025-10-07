#include "bigint.hpp"
#include <algorithm>
#include <ctype.h>
#include <ostream>

bigint::bigint() {
  a.clear();
  a.push_back(0);
}

bigint::bigint(unsigned int v){
  a.clear();
  if(v == 0) { a.push_back(0); return;}
  while( v > 0) {
    a.push_back(static_cast<int>(v % BASE));
    v /= BASE;
  }
}

bigint::bigint(const std::string &s){
  fromString(s);
}


void bigint::fromString(const std::string &s) {
  a.clear();
  int n = static_cast<int>(s.size());
  int i = n;
  
  while( i > 0){
    int start = std::max(0, i - BASE_DIGS);
    int len = i - start;
    int chunk = 0;

    for (int p = start; p < i; ++p) {
      char c = s[p];
      if (!isdigit(static_cast<unsigned char>(c))){
        a.clear(); a.push_back(0);
        normalize();
        return;
      }
      chunk = chunk * 10 + (c - '0');
    }
    a.push_back(chunk);
    i -= len;
  }
  normalize();
}

std::string bigint::toString() const {
  if(a.empty() || (a.size() == 1 && a[0] == 0)) return "0";

  std::ostringstream oss;
  int i  = static_cast<int>(a.size()) - 1;
  
  oss << a[i];

  for (i = i - 1; i >= 0; --i) {
    oss << std::setw(BASE_DIGS) << std::setfill('0') << a[i];
  }
  return oss.str();
}

void bigint::normalize() {
  while(a.size() > 1  && a.back() == 0)
    a.pop_back();
  if (a.empty())
    a.push_back(0);
}

bool bigint::operator==(const bigint &rhs) const {
  return a == rhs.a;
}

bool bigint::operator<(const bigint &rhs) const {
  if (a.size() != rhs.a.size())
    return a.size() < rhs.a.size();

  for (int i = static_cast<int>(a.size()) - 1; i >= 0; --i) {
    if ( a[i] != rhs.a[i])
      return a[i] < rhs.a[i];
  }

  return false;
}

bigint& bigint::operator+=(const bigint& rhs) {
  
  size_t n = a.size();
  size_t m = rhs.a.size();
  size_t L = (n > m) ? n : m;
  a.resize(L, 0);

  int carry = 0;
  for (size_t i = 0; i < L; ++i) {
    long sum = (long)a[i] + (long)(i < rhs.a.size() ? rhs.a[i] : 0) + carry;
    if ( sum >= BASE) {
      a[i] = static_cast<int>(sum - BASE);
      carry = 1;
    } else {
      a[i] = static_cast<int>(sum);
      carry = 0;
    }
  }
  if (carry)
    a.push_back(carry);
  return *this;
}

bigint& bigint::operator++(){
  int carry = 1;
  for (size_t i = 0; i < a.size() && carry; ++i) {
    int v = a[i] + carry;
    if ( v >= BASE ) { a[i] = v - BASE; carry = 1;}
    else {a[i] = v; carry = 0;}
  }
  if (carry) 
    a.push_back(1);
  return *this;
}

bigint bigint::operator++(int) {
  bigint tmp = *this;
  ++(*this);
  return tmp;
}

void bigint::mul_small(int m) {
  if (m == 0) { a.clear(); a.push_back(0); return;}
  if (m == 1) return;

  long carry = 0;
  for (size_t i = 0; i < a.size(); ++i) {
    long cur = (long)a[i] * (long)m + carry;
    a[i] = static_cast<int>(cur % BASE);
    carry = cur / BASE;
  }
  if(carry)
    a.push_back(static_cast<int>(carry));
}

int bigint::div_small(int m){
  long rem = 0;
  for (int i = static_cast<int>(a.size()) - 1;  i >= 0; --i) {
    long cur = a[i] + rem * (long)BASE;
    a[i] = static_cast<int>(cur / m);
    rem = static_cast<int>(cur % m);
  }
  normalize();
  return static_cast<int>(rem);
}

bigint& bigint::operator*=(const bigint &rhs){
  if((a.size() == 1 && a[0] == 0) || (rhs.a.size() == 1 && rhs.a[0] == 0)){
    a.clear(); a.push_back(0); return *this;
  }

  std::vector<int> res(a.size() + rhs.a.size(), 0);

  for (size_t i = 0; i < a.size(); i++) {
    long carry = 0;
    for (size_t j = 0; j < rhs.a.size() || carry; ++j) {
      long cur = res[i + j] + (long)a[i] * (long)(j < rhs.a.size() ? rhs.a[j] : 0) + carry;
      res[i + j] = static_cast<int>(cur % BASE);
      carry = cur / BASE;
    }
  }
  a.swap(res);
  normalize();
  return *this;
}

bigint bigint::operator<<(unsigned int n) const {
  if (n == 0) return *this;
  bigint out = *this;

  unsigned int q = n / BASE_DIGS;
  unsigned int r = n % BASE_DIGS;

  if (!(out.a.size() == 1 && out.a[0] == 0)) {
    if (q > 0)
      out.a.insert(out.a.begin(), q, 0);
    if ( r > 0)
      out.mul_small(pow10_small(static_cast<int>(r)));
  }
  out.normalize();
  return out;
}

bigint bigint::operator>>(unsigned int n) const {
  if (n == 0) return *this;
  bigint out = *this;

  unsigned int q = n / BASE_DIGS;
  unsigned int r = n % BASE_DIGS;

  if ( q >= out.a.size()){
    out.a.clear();
    out.a.push_back(0);
    return out;
  }
  if (q > 0)
    out.a.erase(out.a.begin(), out.a.begin() + q);
  if (r > 0)
    (void)out.div_small(pow10_small(static_cast<int>(r)));
  out.normalize();
  return out;
}

unsigned int bigint::to_uint_saturating(const bigint& n) {
  const unsigned int UMAX = ~0u;
  std::string s = n.toString();
  unsigned long long v = 0;
  for (size_t i = 0; i < s.size(); ++i) {
    v = v * 10 + (s[i] - '0');
    if (v > UMAX) return UMAX;
  }
  return static_cast<unsigned int>(v);
}

bigint bigint::operator<<(const bigint& n) const {
  return (*this) << to_uint_saturating(n);
}

bigint bigint::operator>>(const bigint& n) const {
  return (*this) >> to_uint_saturating(n);
}
