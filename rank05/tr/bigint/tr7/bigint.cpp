#include "bigint.hpp"

bigint::bigint() : a(1, 0){}
bigint::bigint(unsigned int v){t_i(v);}
bigint::bigint(const bigint &other){ *this = other;}
        
void bigint::t_i(unsigned int v){
    if(v == 0) { return a.push_back(0);}
    while(v > 0){
        a.push_back(v % 10);
        v /= 10;
    }
}

