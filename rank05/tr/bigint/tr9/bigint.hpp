#pragma once

#include <iostream>
#include <cctype>
#include <vector>
#include <climits>

class bigint{
  typedef int digit_t;
  private:
    std::vector<digit_t>a;
    void for_num(unsigned int v);
    bool is_zero() const;
    void normalize();

  public:
    bigint();
    bigint(unsigned int v);
    bigint(const bigint &s);
    std::string tostring() const; 
    
    bigint operator+(const bigint &s) const;
    bigint& operator+=(const bigint &s);
    
    bigint& operator++();
    bigint operator++(int);
    
    bigint operator<<(unsigned int num) const;
    bigint operator>>(unsigned int num) const;
    bigint operator<<=(unsigned int num) ;
    bigint operator>>=(unsigned int num) ;
    
    bigint operator<<(const bigint &s) const;
    bigint operator>>(const bigint &s) const;
    bigint operator<<=(const bigint &s);
    bigint operator>>=(const bigint &s);
    static unsigned int to_unit(const bigint &s);

    bool operator==(const bigint &s) const;
    bool operator!=(const bigint &s) const;
    bool operator<(const bigint &s) const;
    bool operator>(const bigint &s) const;
    bool operator<=(const bigint &s) const;
    bool operator>=(const bigint &s) const;


};

std::ostream& operator<<(std::ostream &os, const bigint &x);
