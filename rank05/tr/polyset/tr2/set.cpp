#include "set.hpp"

set::set(searchable_bag &s_bag){
    bag = &s_bag;    
}

set::~set(){};

bool set::has(int value) const { return bag->has(value); }

void set::insert(int value){
    if(!this->has(value))
        bag->insert(value);
}

void set::insert(int *data, int size){
    for(int i = 0; i < size; i++){
        this->insert(data[i]);
    }
}

void set::print() const{
    bag->print();
}

void set::clear(){
    bag->clear();
}

const searchable_bag& set::get_bag() const{
    return *bag;
}

