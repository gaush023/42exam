#include "searchable_tree_bag.hpp"

searchable_tree_bag& searchable_tree_bag::operator=(const searchable_tree_bag &s){
  if(this != &s){
    tree_bag::operator=(s);  
  }
  return *this;
}

bool searchable_tree_bag::search(node *v, const int value) const{
  if(v == nullptr)
    return false;
  if(v->value == value)
    return true;
  if(value < v->value)
    return search(v->l, value);
  else {
      return search(v->r, value);
  }
}

