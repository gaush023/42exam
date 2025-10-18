#pragma once

#include <iostream>
#include <vector>
#include <cctype>
#include <climits>

class bigint{
    public:
        bigint();
        bigint(unsigned int v);
        bigint(const bigint &other);
    private:
        std::vector<int> a;
        void t_i(unsigned int v);
};

