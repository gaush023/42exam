#include "searchable_tree_bag.hpp"

searchable_tree_bag::searchable_tree_bag(){}
searchable_tree_bag::searchable_tree_bag(const searchable_tree_bag &s) : tree_bag(s) {}

searchable_tree_bag& searchable_tree_bag::operator=(const searchable_tree_bag &s){
    if(this != &s)
        tree_bag::operator=(s);
    return *this;
}

bool searchable_tree_bag::search(node *n, const int value) const{
    if(n == nullptr)
        return false;
    if(n->value == value)
        return true;
    if(value < n->value)
       return search(n->l, value);
    else
       return search(n->r, value);
}

bool searchable_tree_bag::has(int value) const{
    return (search(this->tree, value));
}

searchable_tree_bag::~searchable_tree_bag(){}
