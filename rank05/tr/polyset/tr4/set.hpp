#pragma

#include "searchable_bag.hpp"
#include "bag.hpp"

class set{
    private:
        searchable_bag &s_bag;
    public:
        set() = delete();
        set(const set &source) = delete;
        set& operator=(const set &source) = delete;
   
        bool has(int value) const;
        set(searchable_bag &s_bag);
        void insert (int);
        void insert (int *, int);
        void print();
        void clear();
        const searchable_bag &get_bag();
        ~set();

};

