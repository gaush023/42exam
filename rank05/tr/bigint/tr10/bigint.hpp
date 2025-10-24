#pragma once

#include <iostream>
#include <vector>
#include <cctype>
#include <climits>


class bigint{
  typedef int digit_t;
  private:
    std::vector<digit_t> a;
    void t_n(unsigned int v);
    bool is_zero() const;
    void normalize();
    static unsigned int t_u(const bigint &s);
  public:
    bigint(): a(1, 0){}
    bigint(unsigned int v){ t_n(v);}
    bigint(const bigint &s) : a(s.a) {}
    std::string tostring() const;
   
    bigint operator+(const bigint &s) const;
    bigint operator+=(const bigint &s);
    bigint operator++();
    bigint operator++(int);

    bigint operator<<(unsigned int num); 
    bigint operator>>(unsigned int num); 
    bigint operator<<=(unsigned int num); 
    bigint operator>>=(unsigned int num); 
    
    bigint operator<<(const bigint &s); 
    bigint operator>>(const bigint &s); 
    bigint operator<<=(const bigint &s);
    bigint operator>>=(const bigint &s); 
    
    bool operator<(const bigint &s) const;
    bool operator>(const bigint &s) const;
    bool operator<=(const bigint &s) const;
    bool operator>=(const bigint &s) const;
    bool operator!=(const bigint &s) const;
    bool operator==(const bigint &s) const;

};

std::ostream& operator<<(std::ostream &os, const bigint &s);
