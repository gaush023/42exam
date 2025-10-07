#pragma once

#include <vector>
#include <string>
#include <iosfwd>
#include <sstream>
#include <iomanip>
#include <iostream>

class bigint{
  public:
    bigint();
    bigint(unsigned int v);
    explicit bigint(const std::string &s);
    
    std::string toString() const;
    
    bool operator==(const bigint &rhs) const;
    bool operator!=(const bigint &rhs) const { return !(*this == rhs);}
    bool operator<(const bigint &rhs) const;
    bool operator>(const bigint &rhs) const { return rhs < *this; }
    bool operator<=(const bigint &rhs) const { return !(*this > rhs);}
    bool operator>=(const bigint &rhs) const { return !(*this < rhs);}
    
    friend bigint operator+(bigint lhs, const bigint& rhs) { lhs += rhs; return lhs; }
    bigint& operator++();
    bigint operator++(int);

    bigint& operator*=(const bigint& rhs);
    friend bigint operator*(bigint lhs, const bigint& rhs) { lhs *= rhs; return lhs;}
  
    bigint& operator+=(const bigint& rhs);

    bigint operator<<(unsigned int n) const;
    bigint operator>>(unsigned int n) const;
    bigint& operator<<=(unsigned int n) {*this = (*this << n); return *this;}
    bigint& operator>>=(unsigned int n) {*this = (*this >> n); return *this;}

    bigint  operator<<(const bigint& n) const;
    bigint  operator>>(const bigint& n) const;
    bigint& operator<<=(const bigint& n) { *this = (*this << n); return *this; }
    bigint& operator>>=(const bigint& n) { *this = (*this >> n); return *this; }
    static unsigned int to_uint_saturating(const bigint& n);


    friend std::ostream& operator<<(std::ostream& os, const bigint& x){
    os << x.toString(); return os;
    }

  private:
    std::vector<int> a; 
    
    enum { BASE = 10000, BASE_DIGS = 4 };

    void normalize();
    void fromString(const std::string &s);

    void mul_small(int m);
    int div_small(int m);
  
    static int pow10_small(int k) {
      if (k == 0) return 1;
      if (k == 1) return 10;
      if (k == 2) return 100;
      return 1000;
    }
};

