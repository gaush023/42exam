#include "bigint.hpp"

bigint::bigint() : a(1, 0){}
bigint::bigint(unsigned int v){ for_num(v);}
bigint::bigint(const bigint &s) :  a(s.a) {}

void bigint::for_num(unsigned int v){
  a.clear();
  if( v == 0){a.push_back(0);}
  while(v > 0){
    a.push_back(v % 10);
    v /= 10;
  }
}

std::string bigint::tostring() const{
  if(a.size() == 1 && a[0] == 0 ) { return "0";}
  std::string s;
  s.reserve(a.size());
  for(int i = (int)a.size() -1; i >= 0; --i){
    s.push_back(a[i] + '0');
  }
  return s;  
}

bigint bigint::operator+(const bigint &s) const{
  bigint tmp = *this;
  tmp += s;
  return tmp;
}

bigint& bigint::operator+=(const bigint &s){
  int l = (a.size() > s.a.size() ? a.size() : s.a.size());
  a.resize(l, 0);
  int carry = 0;
  for(int i = (int)a.size() - 1; i >= 0; --i){
    int sum = a[i] + (s.a.size() > i ? s.a[i] : 0) + carry;
    a[i] = sum % 10;
    carry = sum/10;
  }
  if(carry)
    a.push_back(carry);
  return *this;
}

bigint& bigint::operator++(){
  *this += bigint(1);
  return *this;
}

bigint bigint::operator++(int){
  bigint tmp = *this;
  ++(*this);
  return tmp;
}

bool bigint::is_zero() const{
  return (a.size() == 1 && a[0] == 0);
}

bigint bigint::operator<<(unsigned int num) const{
  if(is_zero()){ return *this;}
  bigint out;
  out.a = a;
  out.a.insert(out.a.begin(), num, 0);
  return out;
}

void bigint::normalize(){
  while(a.size() > 1 && a.back() == 0){ a.pop_back();}
}

bigint bigint::operator>>(unsigned int num)const{
  if(num >= a.size()){ return bigint(0u);}
  bigint out;
  out.a.assign(a.begin() + num, a.end());
  out.normalize();
  return out;
}

bigint bigint::operator<<=(unsigned int num) {*this = *this << num; return *this;}
bigint bigint::operator>>=(unsigned int num) {*this = *this >> num; return *this;}

unsigned int bigint::to_unit(const bigint &s){
  unsigned int v = 0;
  for(int i = (int)s.a.size() - 1; i >= 0; --i){
    v = v * 10 + s.a[i];
    if(v > UINT_MAX)
      return UINT_MAX;
  }
  return (unsigned int)v;
}


bigint bigint::operator<<(const bigint &s) const{
    return *this << to_unit(s);  
}
bigint bigint::operator>>(const bigint &s) const{
    return *this >> to_unit(s);  
}
bigint bigint::operator<<=(const bigint &num) {*this = *this << num; return *this;}
bigint bigint::operator>>=(const bigint &num) {*this = *this >> num; return *this;}

bool bigint::operator==(const bigint &s)const{ return a ==  s.a;}
bool bigint::operator!=(const bigint &s)const{ return !(*this==s);} 
bool bigint::operator<=(const bigint &s)const{ return !(s < *this);} 
bool bigint::operator>=(const bigint &s)const{ return !(*this < s);} 
bool bigint::operator>(const bigint &s)const{ return (s < *this);} 
bool bigint::operator<(const bigint &s)const{ 
  if(a.size() != s.a.size()){return a.size() < s.a.size();}
  for(int i = (int)a.size() - 1; i >= 0; --i){
    if(a[i] != s.a[i])
      return a[i] < s.a[i];
  }
  return false;
} 


std::ostream& operator<<(std::ostream &os, const bigint &x){
  os << x.tostring();
  return os;
}
