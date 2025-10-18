#pragma once

#include "searchable_bag.hpp"
#include "node_bag.hpp"


class searchable_node_bag : public searchable_bag : public node_bag{
    private:
        bool search(node *n, const int v) const;
    public:
        searchable_node_bag();
        searchable_node_bag(const searchable_node_bag &s);
        searchable_node_bag& operator=(const searchable_node_bag &s);
        bool has(int value) const;
        ~searchable_node_bag();
}

