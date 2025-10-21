#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <climits>


class bigint{
  typedef int digit_t;
  private:
    std::vector<digit_t> a;
    void t_i(unsigned int v);
    bool is_zero() const;
    void normalize();

  public:
    bigint();
    bigint(unsigned int v);
    bigint(const bigint &s);

    std::string tostring() const;

    bigint operator+(const bigint &s) const;
    bigint operator+=(const bigint &s);

    bigint& operator++();
    bigint operator++(int);

    bigint operator>>(unsigned int v) const;
    bigint operator<<(unsigned int v) const;
    bigint operator>>=(unsigned int v);
    bigint operator<<=(unsigned int v);
   
    bool operator<(const bigint &s) const;
    bool operator>(const bigint &s) const;
    bool operator<=(const bigint &s) const;
    bool operator>=(const bigint &s) const;
    bool operator!=(const bigint &s) const;
    bool operator==(const bigint &s) const;


    bigint operator>>(const bigint &s) const;
    bigint operator<<(const bigint &s) const;
    bigint operator>>=(const bigint &s);
    bigint operator<<=(const bigint &s);

    static unsigned int t_s(const bigint &s);
};

std::ostream &operator<<(std::ostream &os, const bigint &s);
