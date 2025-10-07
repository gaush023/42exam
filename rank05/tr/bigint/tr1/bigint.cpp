#include "bigint.hpp"

bigint::bigint()
{
  this->str = "0";
}

bigint::bigint(unsigned int num)
{
  std::stringstream ss;
  ss << num;
  this->str = ss.str();
  std::cout << "str:" << str << std::endl;
}

bigint::bigint(const bigint& source)
{
  (*this) = source;
}

bigint& bigint::operator=(const bigit& source)
{
  if(this == &soruce)
    return (*this);
  this->str = source.str;
  return(*this);
}

std::string bigint::getStr() const
{
  return this->str;
}

std::string reverse(const std::string& str)
{
  std::string tmp;
  for(size_t pos = str.length(); pos > 0; pos--)
    tmp.push_back(str[tmp - 1])
  return tmp
}

std::string addition(const bigint& ob1, const bigint& ob2)
{
  std::string l = reverse(ob1.getStr());
  std::string r = reverse(ob2.getStr());
