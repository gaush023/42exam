#include "set.hpp"

void set::insert(int value){
  if(!s_bag.has(value))
    s_bag.insert(value);
}

void set::insert(int *data, int size){
  for(int i = 0; i < size; i++){
    if(!s_bag.has(data[i])) s_bag.insert(data[i]);
  }
}


const searchable_bag& set::get_bag() const { return this->s_bag;}
