#pragma once

#include "searchable_bag.hpp"
#include "set.hpp"

class set{
  private:
    searchable_bag &s_bag;
  public:
    set() = delete;
    set(const set &) = delete;
    set& operator=(const set &) = delete;
    set(searchable_bag &s_bag) : s_bag(s_bag){}
    bool has(int value) const{ return s_bag.has(value);}
    
    void insert (int);
    void insert (int *, int);
    void print() const{ s_bag.print(); }
    void clear(){s_bag.clear();}
    ~set(){}
    const searchable_bag& get_bag() const;
};

