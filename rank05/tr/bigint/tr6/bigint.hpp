#pragma once

#include <vector>
#include <string>
#include <cctype>
#include <iostream>
#include <climits>

class bigint{
	typedef int digit_t;
	public:
		bigint();
		bigint(unsigned int v);
		bigint(std::string &s);
		bigint(const bigint &others);

		std::string tostring() const;

		bigint operator+(const bigint &rhs) const;
		bigint& operator+=(const bigint &rhs);
		bigint& operator++();
		bigint operator++(int);

		bigint operator<<(unsigned int n) const;
		bigint operator>>(unsigned int n) const;
		bigint operator>>=(unsigned int n);
		bigint operator<<=(unsigned int n);

		bool operator<(const bigint &rhs) const;
		bool operator>(const bigint &rhs) const;
		bool operator==(const bigint &rhs) const;
		bool operator!=(const bigint &rhs) const;
		
		bigint operator<<(const bigint &n) const;
		bigint operator>>(const bigint &n) const;
		bigint operator>>=(const bigint &n);
		bigint operator<<=(const bigint &n);

	private:
		std::vector<digit_t>a;
		void normalize();
		bool is_zero() const;
		void from_str(std::string str);
		void from_int(unsigned int v);
		static unsigned int ft_round(const bigint &s);
};

std::ostream& operator<<(std::ostream& os, const bigint &x);

