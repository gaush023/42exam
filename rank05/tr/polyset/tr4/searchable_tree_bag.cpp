#include "searchable_node_bag.hpp"

searchable_node_bag::searchable_node_bag(){}
searchable_node_bag::searchable_node_bag(const searchable_node_bag &s) : node_bag(s) {}

searchable_node_bag& searchable_node_bag::operator=(const searchable_node_bag &s){
    if(*this != s)
        node_bag::operator=(s);
    return *this;
}

bool searchable_node_bag::search(node *n, const int value) const{
    if(n == nullptr)
        return false;
    if(n->value == value)
        return true;
    if(value < n->value)
       return search(n->l, value);
    else
       return search(n->r, value);
}

bool searchable_node_bag::has(int value) const{
    search(this->tree, value);
}

searchable_node_bag::~searchable_node_bag(){}
