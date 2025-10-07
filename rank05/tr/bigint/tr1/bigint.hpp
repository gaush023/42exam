#pragma once  

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

class bigint 
{
  private:
    std::string str;

  public:
    bigint();
    bigint(const bigint& source)
    bigint(const bigint& source)

    std::string getStr()const;

    bigint& operator=(const bigint& source);
  
    bigint operator+(const bigint& other) const;
    bigint& operator+=(const bigint& other);
    
    bigint& operator++();
    bigint operator++(int);

    bigint operator<<(unsigned int n)const;
    bigint operator>>(unsigned int n)const;
    bigint& operator<<=(unsigned int n);
    bigint& operator>>(unsigned int n);
    
    
