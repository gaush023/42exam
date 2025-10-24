#include "bigint.hpp"

bigint::bigint() : a(1, 0){};
bigint::bigint(unsigned int v){ t_i(v); }
bigint::bigint(const bigint &s) : a(s.a) {}

std::ostream &operator<<(std::ostream &os, const bigint &s){
  os << s.tostring();
  return os;
}

bigint bigint::operator+(const bigint &s) const{
  bigint tmp = *this;
  tmp += s;
  return tmp;
}

bigint bigint::operator+=(const bigint &s){
  size_t l = (a.size() > s.a.size() ? a.size() : s.a.size());
  a.resize(l, 0);
  int carry = 0;
  for(int i = int(a.size()) - 1; i >= 0; --i){
    int sum = a[i] + (i< s.a.size() ? s.a[i] : 0) + carry;
    a[i] = sum % 10;
    carry = sum / 10;
  }
  if (carry)
      a.push_back(carry);
  return *this;
}

bigint bigint::operator++(int){
  bigint tmp = *this;
  ++(*this);
  return tmp;
}

bigint &bigint::operator++(){
  *this += bigint(1);
  return *this;
}

void bigint::normalize(){
  while(a.size() > 1 && a.back() == 0){a.pop_back();};
}

bigint bigint::operator>>(unsigned int v) const{
  if(v > a.size()){ return bigint(0u);}
  bigint out;
  out.a.assign(a.begin() + v, a.end());
  out.normalize();
  return out;
}

bool bigint::is_zero() const{
  return (a.size() == 1 && a[0] == 0);
}

bigint bigint::operator<<(unsigned int v) const{
  if(is_zero()){return *this;}
  bigint out;
  out.a = a;
  out.a.insert(out.a.begin(), v, 0);
  return out;
}


bigint bigint::operator>>=(unsigned int v){ *this = *this >> v; return *this;}
bigint bigint::operator<<=(unsigned int v){ *this = *this << v; return *this;}

unsigned int bigint::t_s(const bigint &s){
  unsigned long v = 0;
  for(int i = s.a.size() - 1; i >= 0; --i){
    v = v * 10 + s.a[i];
    if(v > UINT_MAX)
      return UINT_MAX;
  }
  return (unsigned int) v; 
}

bigint bigint::operator<<(const bigint &s) const{
  return *this << t_s(s);
}

bigint bigint::operator>>(const bigint &s) const{
  return *this >> t_s(s);
}

bigint bigint::operator>>=(const bigint &s){
  *this = *this >> s;
  return *this;
}

bigint bigint::operator<<=(const bigint &s){
  *this = *this << s;
  return *this;
}

bool bigint::operator==(const bigint &s) const {
    return a == s.a;
}

bool bigint::operator<(const bigint &s) const {
    if (a.size() != s.a.size())
        return a.size() < s.a.size();
    for (int i = (int)a.size() - 1; i >= 0; --i) {
        if (a[i] != s.a[i])
            return a[i] < s.a[i];
    }
    return false;
}

bool bigint::operator>(const bigint &s) const {
    return *this < s;
}

bool bigint::operator<=(const bigint &s) const {
    return !(s < *this);
}

bool bigint::operator>=(const bigint &s) const {
    return !(*this < s);
}

bool bigint::operator!=(const bigint &s) const {
    return !(*this == s);
}

std::string bigint::tostring() const{
  if(a.size() == 0){ return "0"; }
  std::string s;
  s.reserve(a.size());
  for(int i = (int)a.size() ; i >= 0; --i)
    s.push_back(char(a[i] + '0'));
  return s;
}

void bigint::t_i(unsigned int v){
  a.clear();
  if(v == 0){ a.push_back(0); return;}
  while(v > 0){
    a.push_back(v % 10);
    v /= 10;
  }
}
