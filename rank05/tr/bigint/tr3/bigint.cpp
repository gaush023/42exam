#include "bigint.hpp"

#include <cctype>
#include <climits>

bigint::bigint() : a(1, 0) {}
bigint::bigint(unsigned int v) { from_unit(v); }
bigint::bigint(const std::string &s) { from_string(s);}
bigint::bigint(const bigint &other) : a(other.a) {}

std::string bigint::tostring() const {
	if(a.size() == 1 && a[0] == 0) return "0";
	std::string s;
	s.reserve(a.size());
	for(int i = (int)a.size() - 1; i >= 0; --i)
		s.push_back(char('0' + a[i]));
	return s;
}

std::ostream& operator<<(std::ostream& os, const bigint& x) {
	os << x.tostring();
  return os;
}

bool bigint::operator==(const bigint& rhs) const { return a == rhs.a; }
bool bigint::operator!=(const bigint& rhs) const { return !(*this == rhs);}
bool bigint::operator<(const bigint& rhs) const { 
	if (a.size() !=  rhs.a.size()) return a.size() < rhs.a.size();
	for(int i = (int)a.size() - 1; i >= 0; --i)
		if (a[i] != rhs.a[i]) return a[i] < rhs.a[i];
	return false;
}
bool bigint::operator>(const bigint& rhs) const { return rhs < *this;}
bool bigint::operator<=(const bigint& rhs) const {
    return (*this < rhs) || (*this == rhs);
}
bool bigint::operator>=(const bigint& rhs) const {
    return (*this > rhs) || (*this == rhs);
}


bigint& bigint::operator+=(const bigint& rhs) {
	size_t l = (a.size() > rhs.a.size()) ? a.size() : rhs.a.size();
	a.resize(l, 0);
	int carry = 0;
	for(size_t i = 0; i < l; ++i){
		int sum = a[i] + (i < rhs.a.size() ? rhs.a[i] : 0) + carry;
		if (sum >= 10){
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

bigint bigint::operator+(const bigint& rhs) const {
	bigint tmp = *this;
	tmp += rhs;
	return tmp;
}

bigint& bigint::operator++() {
    *this = *this + bigint(1);
    return *this;
}

bigint bigint::operator++(int) {
	bigint tmp = *this;
	++(*this);
	return tmp;
}

bigint bigint::operator<<(unsigned int n) const {
	if (is_zero()) return *this;
	bigint out;
  out.a = a;
	out.a.insert(out.a.begin(), n, 0);
	return out;
}

bigint bigint::operator>>(unsigned int n) const {
	if(n >= a.size()) return bigint(0u);
	bigint out;
	out.a.assign(a.begin() + n, a.end());
	out.normalize();
	return out;
}

bigint& bigint::operator<<=(unsigned int n)  { *this = *this << n; return *this; }
bigint& bigint::operator>>=(unsigned int n)  { *this = *this >> n; return *this; }

bigint bigint::operator<<(const bigint& n) const{
	return (*this) << to_uint_satruating(n);
}

bigint bigint::operator>>(const bigint& n) const{
	return (*this) >> to_uint_satruating(n);
}

bigint bigint::operator<<=(const bigint& n){
	*this = *this << n;
	return *this;
}

bigint bigint::operator>>=(const bigint& n){
	*this = *this >> n;
	return *this;
}

bool bigint::is_zero() const { return (a.size() == 1 && a[0] == 0); }

void bigint::normalize(){
	while(a.size() > 1 && a.back() == 0) a.pop_back();
}

void bigint::from_unit(unsigned int v){ 
	a.clear();
	if ( v == 0 ) { a.push_back(0); return;}
	while(v > 0){
		a.push_back(digit_t(v % 10));
		v /= 10;
	}
}

void bigint::from_string(const std::string &s){
	a.clear();
	size_t i = 0;
	while( i < s.size() && s[i] == '0') ++i;
	if (i == s.size()) {
		a.clear();
		a.push_back(0);
		return;
	}
	for(size_t p = s.size(); p > i; --p){
		char c = s[p - 1];
		if (!std::isdigit((unsigned char)c)){
			a.clear();
			a.push_back(0);
			return;
		}
		a.push_back(digit_t(c - '0'));
	}
	normalize();
}
       
unsigned int bigint::to_uint_satruating(const bigint& n){
	unsigned long v = 0;
	for (int i = (int)n.a.size() - 1; i >= 0; --i){
		v = v * 10 + n.a[i];
		if( v > UINT_MAX) return UINT_MAX;
	}
	return (unsigned int)v;
}


