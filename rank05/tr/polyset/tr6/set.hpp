#pragma once

#include "bag.hpp"
#include "searchable_bag.hpp"

class set{
    searchable_bag &s_bag;
  public:
    set() = delete;
    set(const set &) = delete;
    set& operator=(const set &) = delete;

    set(searchable_bag &s_bag);
   
    bool has(int) const;
    void insert (int);
    void insert (int *, int);
    void print() const;
    void clear();
    const searchable_bag &get_bag();
    ~set() {}
  
};
