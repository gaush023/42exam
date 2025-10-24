#include "set.hpp"

set::set(searchable_bag &s_bag) : s_bag(s_bag) {}
bool set::has(int value) const {return s_bag.has(value);}
void set::insert(int value){
  if(!s_bag.has(value))
    s_bag.insert(value);
}
void set::insert(int *data, int size){
  for(int i = 0; i < size; i++)
    s_bag.insert(data[i]);
}
void set::print() const{ s_bag.print();}
void set::clear() { s_bag.clear();}
const searchable_bag& set::get_bag() const{ return s_bag;}
set::~set(){}
