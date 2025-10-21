#pragma once

#include "searchable_bag.hpp"
#include "bag.hpp"

class set{
  private:
    searchable_bag &s_bag;
  public:
    set() = delete;
    set(const set &) = delete;
    set& operator=(const set &) = delete;

    set(searchable_bag &s_bag) : s_bag(s_bag) {}
    void insert(int *, int);
    void insert(int);
    void clear();
    bool has(int) const;
    void print() const;

    const searchable_bag &get_bag();
    ~set(){}

};
