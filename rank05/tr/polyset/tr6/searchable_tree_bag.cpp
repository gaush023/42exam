#include "searchable_tree_bag.hpp"

searchable_tree_bag& searchable_tree_bag::operator=(const searchable_tree_bag &s){
  if(this != &s){
    tree_bag::operator=(s);
  }
  return *this;
}

bool searchable_tree_bag::search(node *node,const int value) const{
  if(node == nullptr)
      return false;
  if(node->value == value)
      return true;
  if(value < node->value)
      return search(node->l, value);
  return search(node->r, value);
}

bool searchable_tree_bag::has(int value) const{
  return search(this->tree,value);
}
