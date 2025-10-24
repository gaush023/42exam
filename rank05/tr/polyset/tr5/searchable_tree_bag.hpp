#pragma

#include "searchable_bag.hpp"
#include "tree_bag.hpp"

class searchable_tree_bag : public tree_bag, public searchable_bag{
  private:
    bool search(node *node, const int value) const;
  public:
    searchable_tree_bag();
    searchable_tree_bag(const searchable_tree_bag &s);
    searchable_tree_bag& operator=(const searchable_tree_bag &s);
    bool has(int value) const;
    ~searchable_tree_bag();
};
