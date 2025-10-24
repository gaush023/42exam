#include "bigint.hpp"

void bigint::t_n(unsigned int v){
  a.clear();
  if( v == 0){ a.push_back(0);}
  while(v > 0){
    a.push_back(v % 10);
    v /= 10;
  }
}

std::string bigint::tostring() const{
  if(a.size() == 1 && a[0] == 0) return "0";
  std::string s;
  s.reserve(a.size());
  for(int i = (int)a.size() -1; i >= 0; --i){
    s.push_back(char(a[i] + '0'));
    }
  return s;
}

std::ostream& operator<<(std::ostream &os, const bigint &s){
  os << s.tostring();
  return os;
}

bigint bigint::operator+(const bigint &s) const{
  bigint tmp = *this;
  tmp += s;
  return tmp;
}

bigint bigint::operator+=(const bigint &s){
  int l = int(a.size() > s.a.size() ? a.size() : s.a.size());
  a.resize(l, 0);
  int carry = 0;
  for(int i = 0; i < l; ++i){
    int sum = a[i] + (i < s.a.size() ? s.a[i] : 0) + carry;
    a[i] = sum % 10;
    carry = sum / 10;
  }
  if(carry)
    a.push_back(carry);
  return *this;
}

bigint bigint::operator++(){
  *this += bigint(1);
  return *this;
}

bigint bigint::operator++(int){
  bigint tmp = *this;
  ++(*this);
  return tmp;
}

bool bigint::is_zero() const{
  return(a.size() == 1 && a[0] == 0);
}

bigint bigint::operator<<(unsigned int num){
  if(is_zero()){ return *this;}
  bigint out;
  out.a = a;
  out.a.insert(out.a.begin(), num, 0);
  return out;
}

void bigint::normalize(){
  while(a.size() > 1 && a.back() == 0){ a.pop_back();}
}

bigint bigint::operator>>(unsigned int num){
  if(num > a.size()) { return bigint(0u);}
  bigint out;
  out.a.assign(a.begin() + num, a.end());
  out.normalize();
  return *this; 
}


bigint bigint::operator<<=(unsigned int num){ return *this = *this << num;}
bigint bigint::operator>>=(unsigned int num){ return *this = *this >> num;}

unsigned int bigint::t_u(const bigint &s){
  unsigned int v = 0;
  for(int i = (int)s.a.size() - 1; i >= 0; --i){
    v = v * 10 + s.a[i];
    if(v > UINT_MAX)
      return UINT_MAX;
  }
  return (unsigned int)v;
}

bigint bigint::operator<<(const bigint &num){ return *this << t_u(num);}
bigint bigint::operator>>(const bigint &num){ return *this >> t_u(num);}
bigint bigint::operator<<=(const bigint &num){ return *this = *this << num;}
bigint bigint::operator>>=(const bigint &num){ return *this = *this >> num;}

bool bigint::operator<(const bigint &s) const {
  if(a.size() != s.a.size()){return a.size() < s.a.size(); }
  for(int i = (int)a.size() - 1; i >= 0; --i){
    if(a[i] != s.a[i])
      return a[i] < s.a[i];
  }
  return false;
}
bool bigint::operator>(const bigint &s) const{ return s < *this;}
bool bigint::operator<=(const bigint &s) const{
  return *this < s || *this == s; 
}
bool bigint::operator>=(const bigint &s) const{
  return *this > s || *this == s;
}
bool bigint::operator!=(const bigint &s) const{ 
  return !(*this == s);
}
bool bigint::operator==(const bigint &s) const{
  return a == s.a;
}
