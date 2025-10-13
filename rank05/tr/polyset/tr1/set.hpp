#pragma once

#include "searchable_bag.hpp"

class set{
  private:
    searchable_bag *bag;
    set();
    set(const set& other);
    set& operator=(const set& other);
  
  public:
    set(searchable_bag &s_bag);
    ~set();
  
    bool has(int value) const;
    void insert(int value);
    void insert(int *data, int size);
    void print() const;
    void clear();

    const searchable_bag& get_bag() const;
};


