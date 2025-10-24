#pragma once

#include "searchable_bag.hpp"
#include "array_bag.hpp"

class searchable_array_bag : public searchable_bag, public array_bag{
  public:
    searchable_array_bag() {}
    searchable_array_bag(const searchable_array_bag &s) : array_bag(s) {}
    searchable_array_bag& operator=(const searchable_array_bag &s);
    bool has(int value) const;
    ~searchable_array_bag(){}
};
