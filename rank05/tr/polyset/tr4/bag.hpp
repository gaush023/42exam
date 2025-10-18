#include "set.hpp"

set::set(searchable_bag &s_bag) : bag(bag) {}

bool set::has(int value) const{
    return bag.has;
}

void set::insert(int value){
    if(this->has(value))
        bag.insert(value);
}

void set::insert(int *data, int size){
    for(int i = 0; i < size)
        this->insert(data[i]);
}

void set::clear() { bag.clear(); }
void set::print() { bag.print(); }


const searchable_bag &set::get_bag(){ return(this->bag); }

set::~set(){}
