#pragma once

#include <vector>
#include <iostream>
#include <cctype>
#include <climits>
#include <algorithm>

class bigint{
    typedef int digi_t;
    private:
        std::vector<digi_t> a;
        void f_n(unsigned int v);
        bool is_zero() const;
        void nomalize();
        static unsigned int t_u(const bigint &s);
    
    public:
        bigint() : a(1, 0) {}
        bigint(unsigned int v){  f_n(v);}
        bigint(const bigint &s) : a(s.a) {}
        std::string tostring() const;

        bigint operator+(const bigint &s) const;
        bigint operator+=(const bigint &s);
        bigint operator++();
        bigint operator++(int);
        
        bigint operator<<(unsigned int v);
        bigint operator>>(unsigned int v);
        bigint operator<<=(unsigned int v){ *this = *this << v; return *this;}
        bigint operator>>=(unsigned int v){ *this = *this >> v; return *this;}

        bigint operator<<(const bigint &s){ return *this << t_u(s);}
        bigint operator>>(const bigint &s){ return *this >> t_u(s);}
        bigint operator<<=(const bigint &s){ return *this = *this << s;}
        bigint operator>>=(const bigint &s){ return *this = *this >> s;}

        bool operator==(const bigint &s) const{ return a == s.a; }
        bool operator!=(const bigint &s) const{ return !(*this == s);}
        bool operator<(const bigint &s) const;
        bool operator>(const bigint &s) const{ return s < *this;}
        bool operator<=(const bigint &s) const{ return *this == s || *this < s;}
        bool operator>=(const bigint &s) const{ return *this == s || s < *this;}
        std::string getStr() const{ return tostring();}

};


std::ostream& operator<<(std::ostream &os, const bigint &s);
