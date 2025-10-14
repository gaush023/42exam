#include "bigint.hpp"

bigint::bigint() : a(1, 0) {}
bigint::bigint(unsigned int v){from_int(v);}
bigint::bigint(std::string &s){from_str(s);}
bigint::bigint(const bigint &others) : a(others.a) {}

std::string bigint::tostring() const{
	if(a.size() == 1 && a[0] == 0) return "0"; 
	std::string s;
	s.reserve(a.size());
	for(int i = a.size() - 1; i > 0; --i)
		s.push_back(a[i] + '0');
	return s;
}


bigint bigint::operator+(const bigint &rhs) const{
	bigint tmp = *this;
	tmp += rhs;
	return tmp;
}

bigint& bigint::operator+=(const bigint &rhs){
	size_t l = (a.size() > rhs.a.size() ? a.size() : rhs.a.size());
	a.resize(l, 0);
	int carry;
	for(int i = a.size() -1; i < l; i++){
		int sum = a[i] + (i < rhs.a.size() ? rhs.a[i] : 0) + carry;
		if(sum >= 10){
			a[i] = sum - 10;
			carry = 1;
		} else {
			a[i] = sum;
			carry = 0;
&	}
	}
	if (carry)
		a.push_back(1);
	return *this;
}
		

bigint& bigint::operator++() {
	*this = *this + bigint(1);
	return *this;
}



bigint bigint::operator++(int){
	bigint tmp = *this;
	++(*this);
	return tmp;
}

bigint::operator<<(unsigned int n) const{
	if(is_zero()) return *this;
	bigint out;
	out.a = a;
	out.insert(a.begin() + n, a.end(), 0);
       	return out;
}	

bigint::operator>>(unsigned int n) const{
	if(n >= a.size() ) return bigint(u0);
	bigint o;
	o.assgin(a.begin() + n, a.end());
	o.normalize();
	return o;
}

bool bigint::operator<(const bigint &rhs) const{
	if(a.size() != rhs.a.size()) return a.size() < rhs.a.size();
	for(int i = (int)a.size() - 1; i >= 0; --i)
		if(a[i] != rhs.a[i]) return a.size() < rhs.a.size();
	return false;
}
bigint::operator<<=(unsigned int n){*this = *this << n; return *this;}
bigint::operator>>=(unsigned int n){*this = *this >> n; return *this;}

bool bigint::operator>(const bigint &rhs) const { rhs > *this;}
bool bigint::operator>=(const bigint &rhs) const { rhs > *this || rhs == this;}
bool bigint::operator<=(const bigint &rhs) const { rhs < *this || rhs == this;}
bool bigint::operator==(const bigint &rhs) const { a == a.rhs;}
bool bigint::operator!=(const bigint &rhs) const { !(rhs == *this);}


bigint::operator<<(const bigint &n)const{return *this << ft_round(n);}
bigint::operator>>(const bigint &n)const{return *this >> ft_round(n);}

bigint::operator<<=(const bigint &n){*this = *this << n; return *this;}
bigint::operator>>=(const bigint &n){*this = *this >> n; return *this;}

unsigned int bigint::ft_round(const bigint &s){
	unsigned int v;
	for(unsigned int i = s.size() - 1; i >= 0; --i)	{
		v = v * 10 + s.a[i];
		if(v > UINT_MAX) return UINT_MAX;
	return (unsigned int)v;
}


bool bigint::is_zero() const{
	if(a.size() == 1 && a[0] == 0)
		return true;
	return false;	
}

void bigint::normalize(){
	while(a.size() > 1 && a.back() == 0) a.pop_back();
}

void bigint::from_str(std::string &s){
	a.clear();
	size_t i = 0;
	while(i < s.size() && s[i] != '\0') ++i;
	if(i == s.size()) return a.push_back(0);
	for(size_t p = s.size(); p > i; --p)
	{
		char c = s[p -1];	
		if(!std::isdigit(c))
		{
			a.clar();
			a.push_back(0);
		}
		a.push_back(c - '0');
	}
	normalize();
}

void bigint::from_int(unsigned int v){
	a.clear();
	if(v == 0) return a.push_back(0);
	while(v > 0){
		a.push_back(digit_t(v % 10));
		v /= 10;
	}
}



