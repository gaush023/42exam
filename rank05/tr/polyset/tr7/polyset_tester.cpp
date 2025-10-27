#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

#include "searchable_array_bag.hpp"
#include "searchable_tree_bag.hpp"
#include "set.hpp"

namespace
{

void require(bool condition, const std::string &message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

template <typename Printable>
std::string capture_print(Printable &object)
{
    std::ostringstream oss;
    std::streambuf *previous = std::cout.rdbuf(oss.rdbuf());
    object.print();
    std::cout.rdbuf(previous);
    return oss.str();
}

std::vector<int> parse_ints(const std::string &output)
{
    std::vector<int> values;
    std::istringstream iss(output);
    int value = 0;
    while (iss >> value)
    {
        values.push_back(value);
    }
    return values;
}

void test_searchable_array_bag()
{
    searchable_array_bag bag;
    bag.insert(5);
    bag.insert(-2);
    require(bag.has(5), "searchable_array_bag must find inserted values");
    require(bag.has(-2), "searchable_array_bag must find negative values");
    require(!bag.has(7), "searchable_array_bag must report missing values");

    const searchable_array_bag &const_ref = bag;
    require(const_ref.has(-2), "searchable_array_bag::has should be const");

    searchable_array_bag copied(const_ref);
    require(copied.has(5), "Copy constructor should copy contents");
    require(copied.has(-2), "Copy constructor should copy all values");

    bag.insert(42);
    require(bag.has(42), "Newly inserted value should be present");
    require(!copied.has(42), "Copy must be deep (independent storage)");

    searchable_array_bag assigned;
    assigned.insert(99);
    assigned = bag;
    require(assigned.has(5) && assigned.has(42), "Assignment should copy all elements");
    bag.insert(100);
    require(!assigned.has(100), "Assignment must be deep copy");

    std::string printed = capture_print(bag);
    std::vector<int> values = parse_ints(printed);
    require(values.size() == 4, "Array bag print should include all elements");
    require(values[0] == 5 && values[1] == -2 && values[2] == 42 && values[3] == 100,
            "Array bag print should preserve insertion order");

    bag.clear();
    require(!bag.has(5), "Array bag clear must remove all values");
    std::string cleared = capture_print(bag);
    require(parse_ints(cleared).empty(), "Array bag print should be empty after clear");
}

void test_searchable_tree_bag()
{
    searchable_tree_bag tree;
    tree.insert(5);
    tree.insert(2);
    tree.insert(8);
    tree.insert(7);
    tree.insert(9);

    require(tree.has(5), "Tree bag must find root value");
    require(tree.has(2), "Tree bag must find left branch values");
    require(tree.has(9), "Tree bag must find right branch values");
    require(!tree.has(6), "Tree bag must report values that are not present");

    const searchable_tree_bag &const_tree = tree;
    require(const_tree.has(7), "searchable_tree_bag::has should be const");

    searchable_tree_bag copied(const_tree);
    tree.clear();
    require(copied.has(5) && copied.has(2) && copied.has(9),
            "Copy constructor should own an independent copy of the tree");

    searchable_tree_bag assigned;
    assigned.insert(100);
    assigned = copied;
    copied.clear();
    require(assigned.has(5) && assigned.has(2) && assigned.has(9),
            "Assignment operator should copy the tree structure");
    require(!assigned.has(100), "Assignment should replace old tree contents");

    assigned.clear();
    require(!assigned.has(5), "Clear should remove all nodes from tree bag");
}

void test_set_with_array_bag()
{
    searchable_array_bag backing;
    set s(backing);

    require(!s.has(1), "Empty set should not contain elements");

    s.insert(1);
    s.insert(1);
    s.insert(2);
    require(s.has(1) && s.has(2), "Set should contain inserted elements");
    require(!s.has(3), "Set should report values not present");

    const set &const_set = s;
    const searchable_bag *wrapped_ptr = &const_set.get_bag();
    searchable_bag &backing_iface = backing;
    const searchable_bag *backing_ptr = &backing_iface;
    require(wrapped_ptr == backing_ptr, "set::get_bag must return the wrapped bag reference");

    std::string output = capture_print(backing);
    std::vector<int> values = parse_ints(output);
    require(values.size() == 2, "Set must reject duplicate insertions (single values)");
    require(values[0] == 1 && values[1] == 2,
            "Set backed by array should keep first occurrence order");

    int more[] = {2, 3, 3, 4, 1};
    s.insert(more, static_cast<int>(sizeof(more) / sizeof(more[0])));
    output = capture_print(backing);
    values = parse_ints(output);
    require(values.size() == 4, "Set::insert(array) must skip duplicates");
    require(values[0] == 1 && values[1] == 2 && values[2] == 3 && values[3] == 4,
            "Set::insert(array) should add only new values");

    s.clear();
    require(!s.has(1), "Set::clear must remove elements from the set");
    require(!backing.has(1), "Set::clear must clear the wrapped bag");
    output = capture_print(backing);
    values = parse_ints(output);
    require(values.empty(), "Wrapped bag should be empty after set::clear");
}

void test_set_with_tree_bag()
{
    searchable_tree_bag backing;
    set s(backing);

    s.insert(5);
    s.insert(2);
    s.insert(7);
    s.insert(5);
    require(s.has(5) && s.has(2) && s.has(7), "Set should insert values into tree bag");

    int values[] = {1, 9, 7, 9, 3};
    s.insert(values, static_cast<int>(sizeof(values) / sizeof(values[0])));
    require(s.has(1) && s.has(9), "Set should insert new values via array overload");
    require(s.has(3), "Set should insert values not previously present");

    const set &const_set = s;
    const searchable_bag *wrapped_ptr = &const_set.get_bag();
    searchable_bag &backing_iface = backing;
    const searchable_bag *backing_ptr = &backing_iface;
    require(wrapped_ptr == backing_ptr, "set::get_bag should return the same tree bag instance");

    backing.insert(11);
    require(s.has(11), "Set must observe changes made directly to the wrapped bag");

    s.clear();
    require(!s.has(5) && !backing.has(5), "Clear should empty the wrapped tree bag");
    s.insert(4);
    require(s.has(4), "Set should accept new values after being cleared");
}

} // namespace

int main()
{
    try
    {
        test_searchable_array_bag();
        test_searchable_tree_bag();
        test_set_with_array_bag();
        test_set_with_tree_bag();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Test failure: " << ex.what() << std::endl;
        return 1;
    }

    std::cout << "All Polyset tests passed" << std::endl;
    return 0;
}
