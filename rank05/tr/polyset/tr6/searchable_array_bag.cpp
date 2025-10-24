#include "searchable_array_bag.hpp"

searchable_array_bag& searchable_array_bag::operator=(const searchable_array_bag &s){
  if(this != &s){
    array_bag::operator=(s);
  }
  return *this;
}

bool searchable_array_bag::has(int value) const{
  for(int i = 0; i < this->size; i++){
    if(this->data[i] == value)
        return true;
  }
  return false;
}


