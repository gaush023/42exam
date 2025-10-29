#pragma once
#include "tree_bag.hpp"
#include "searchable_bag.hpp"

class searchable_tree_bag : public tree_bag, public searchable_bag {
public:
    searchable_tree_bag() {}
    searchable_tree_bag(const searchable_tree_bag &s) : tree_bag(s) {}
    searchable_tree_bag& operator=(const searchable_tree_bag &s);  // ✅ クラス名を外す
    bool has(int value) const { return search(this->tree, value); }
    ~searchable_tree_bag() {}  // ✅ クラス内に正しく置く

private:
    bool search(node *n, const int value) const;
};

