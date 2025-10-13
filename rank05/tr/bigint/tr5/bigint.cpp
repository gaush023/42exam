#include "bigint.hpp"

bigint::bigint(): a(1,0) {}
bigint::bigint(unsigned int v){from_unit(v);}
bigint::bigint(const std::string &s){from_str(s);}
bigint::bigint(const bigint &other):a(other.a){} 

std::string bigint::tostring() const{
  if(a.size() == 1 && a[0] == '0'){ return '0';}
  std::string s;
  s.reserve(a.size(), 0);
  for(int n = (int)a.size() -1; n >= 0; --n){
    s.push_back(char('0' + a[n]));  
  return s;
}


bigint bigint::operator==(const bigint &rhs) const{ return a == rhs.a; }
bigint bigint::operator!=(const bigint &rhs) const{ return !(*this == rhs); }
bigint bigint::operator<(const bigint &rhs) const{
  if(a.size() != rhs.a.size()){ return a.size() < rhs.a.size();}
  for(int i = a.size() -1; i >= 0; --i)
    if(a[i] != rhs.a[i]) return a[i] < rhs.a[i];
  return false;
}
bigint bigint::operator>(const bigint &rhs) const{ return *this < rhs; }
bigint bigint::operator<=(const bigint &rhs) const{ return *this < rhs || rhs == *this;}
bigint bigint::operator>=(const bigint &rhs) const{ return *this > rhs || *this == rhs;}

bigint bigint::operator+=(const bigint &rhs){
  size_t l = (a.size() > rhs.a.size() ? a.size() : rhs.a.size());
  a.resize(l, 0);
  int carry = 0;
  for(size_t i = 0; i < l; i++){
    int sum = a[i] + (i < rhs.a.size() ? rhs.a[i] : 0) + caryy;
    if( sum >= 10)
    {
      a[i] = digit_t(sum - 10);
      carry = 1;
    } else {
      a[i] = digit_t(sum);
      carry = 0;
    }
  }
  if (carry)
    a.push_back(1);
  return *this;
}

bigit bigint::operator+(const bigint& rhs) const{
  bigint tmp = *this;
  tmp += rhs;
  reutrn tmp;
}

bigint bigint::operator++(){
  bigint tmp = *this;
  return *this;
}

bigint bigint::operator++(int){
  bigit tmp = *this;
  ++(*this);
  return tmp;
}


bigint bigint::operator<<(unsigned int n) const{
  if(is_zero()) return *this;
  bigint o;
  o.a = a;
  o.a.insert(o.a.begin(), n, 0);
  return o;
}

bigint bigint::operator>>(unsigned int n) const{
  if(n >= a.size()) return bigint(u0);
  bigint o;
  o.a.assign(a.o.begin() + n, a.end());
  o.normalize();
  return o;
}




void bigint::from_unit(unsigned int v){
  a.clear();
  if( v == 0){ a.push_back(0); return;}
  while(v > 0){
    a.push_back(digit_t(v % 10));
    v /= 10;
  }
}

void bigint::from_str(const std::string &s){
  a.clear();
  size_t i = 0;
  while(i < s.size() && s[i] == '0') i++;
  if(i == s.size()){
    a.clear();
    a.push_back(0);
    return;
  }
  for(size_t p = s.size(); p > 0; --p){
    char c = s[p -1];
    if(!std::isdigit((unsigned char)c)){
      a.clear();
      a.push_back(0);
      return;
    }
    a.push_back(digit_t(c - '0'));
  }
  normalize();
}

void bigint::normalize(){
  while(a.size() > 1 && a.back() == 0){ a.pop_back();}
}
