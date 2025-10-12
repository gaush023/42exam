#pragma once 

#include <string>
#include <iostream>
#include <vector>

class bigint{
	typedef unsigned char digit_t;
	public:
		bigint();
		bigint(unsigned int v);
		bigint(const std::string &s);
		bigint(const bigint& others);

		std::string tostring() const;

		bool operator==(const bigint& rhs) const;
		bool operator!=(const bigint& rhs) const;
		bool operator<(const bigint& rhs) const;
		bool operator>(const bigint& rhs) const;
		bool operator>=(const bigint& rhs) const;
		bool operator<=(const bigint& rhs) const;

		bigint& operator+=(const bigint& rhs);
		bigint operator+(const bigint& rhs) const;
		bigint& operator++();
		bigint operator++(int);

		bigint operator<<(unsigned int n) const;
		bigint operator>>(unsigned int n) const;
		bigint &operator<<=(unsigned int n);
		bigint &operator>>=(unsigned int n);

		bigint operator<<(const bigint& n) const;
		bigint operator>>(const bigint& n) const;
		bigint operator<<=(const bigint& n);
		bigint operator>>=(const bigint& n);

	private:
		std::vector<digit_t> a;
		bool is_zero() const;
		void normalize();
		void from_unit(unsigned int v);
		void from_string(const std::string &s);
		static unsigned int to_uint_satruating(const bigint& n);
};

std::ostream& operator<<(std::ostream& os, const bigint& x);

