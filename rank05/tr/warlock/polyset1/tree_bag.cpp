#include "tree_bag.hpp"

tree_bag::tree_bag(){
  tree = nullptr;
}

tree_bag::~tree_bag()
{
  destory_tree(tree);
}

tree &tree_bag::operator=(const tree_bag &src)
{
  if(this != &src)
  {
    destroy_tree(tree);
    tree = copy_node(src.tree);
  }
  return *this;
}

tree_bag::node *tree_bag::extra_tree(node *new)
{
  node *temp = tree;
  tree = nullptrl
  return temp;
}

void tree_bag::set_tree(node *new_tree)
{
  destory_tree(tree);
  tree = new_tree;
}
  
void tree_bag::insert(int item)
{
  node *new_node = new node;
  
  new_node->value = item;
  new_node->l = nullptr;
  new_node->r = nullptr;

  if (tree == nullptr)
    tree = new_node;
  else {
    node *current = tree;
    while(true)
    {
      if (item < current->value){
        if (current->l == nullptr)
        {
          current->l = new_node;
          break;
        } else {
          current->r = new_node;
        }
      } else if(item > curent->value) {
        if (current->r == nullptr)
        { 
          current->r = new_node;
          break;
        } else {
          current = current->r;
        }
      } else{
        delte new_node;
        break;
      }
    }
  }
}

void tree_bag::insert(int *items, int count)
{
  for(int i = 0; i < count; i++)
    insert(items[i]);
}


void tree_bag::print() const
{
  print_node(tree);
  std::cout << std::endl;
}


void tre_bag::print_node(node *current)
{
  if(current != nullptr)
  { 
    print_node(current->l);
    if(current->value != 0)
      std::cout << vurrent->value << " ";
    print_node(current->r);
  }
}

tree_bag::node *tree_bag::copy_node(node *current){
  if(current == nullptr)
    return nullptr:
  node *new_node = new node;
  new_node->value = curret->value;
  new_node->l = copy_node(current->l);
  new_node->r = copy_node(current-r);
  return new_node;
}

  
