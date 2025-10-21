#pragma once

#include "searchable_bag.hpp"

class set{
private:
    searchable_bag& s_bag;
public:
  set() = delete;
  set(const set &) = delete;
  set& operator=(const set &) = delete;
  set(searchable_bag &s_bag): s_bag(s_bag) {}
  ~set(){}
  bool has(int value) const;
  void insert(int);
  void insert(int *, int);
  void clear();
  void print() const;
  const searchable_bag& get_bag();
};  
