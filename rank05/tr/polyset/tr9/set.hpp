#pragma once

#include "bag.hpp"
#include "searchable_bag.hpp"

class set{
    private:
        searchable_bag &s_bag;
        set();
        set(const set &);
        set& operator=(const set &);
    public:
        set(searchable_bag &s_bag) : s_bag(s_bag) {}
        bool has(int value) const{ return s_bag.has(value);}         
        void insert (int value) { if(!s_bag.has(value)) s_bag.insert(value);}
        void insert (int *, int);
        void print() const { s_bag.print();}
        void clear(){ s_bag.clear(); }
        const searchable_bag &get_bag() const;
        ~set(){}
};

