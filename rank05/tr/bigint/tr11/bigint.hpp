#pragma once

#include <iostream>
#include <vector>
#include <cctype>
#include <climits>

class bigint{
    typedef int digit_t;
private:
        std::vector<digit_t>a;
        void f_n(unsigned int v);
        bool is_zero() const;
        void normalize();
        static unsigned int t_u(const bigint &s);
    public:
        bigint() : a(1, 0){}
        bigint(unsigned int v){f_n(v);}
        bigint(const bigint &s) { a = s.a; }
        std::string tostring() const;

        bigint operator+(const bigint &s) const;
        bigint operator+=(const bigint &s);
        bigint operator++();
        bigint operator++(int);

        bigint operator<<(unsigned int v);
        bigint operator>>(unsigned int v);
        bigint operator<<=(unsigned int v);
        bigint operator>>=(unsigned int v);
        
        bigint operator<<(const bigint &s);
        bigint operator>>(const bigint &s);
        bigint operator<<=(const bigint &s);          
        bigint operator>>=(const bigint &s);          

        bool operator==(const bigint &s) const{ return a == s.a;}
        bool operator!=(const bigint &s) const{ return !(*this == s);}
        bool operator<(const bigint &s) const;
        bool operator>(const bigint &s) const{return s < *this;}
        bool operator<=(const bigint &s) const{return *this == s || *this < s;}
        bool operator>=(const bigint &s) const{return *this == s || s< *this; }
};


std::ostream& operator<<(std::ostream &os, const bigint &s);
