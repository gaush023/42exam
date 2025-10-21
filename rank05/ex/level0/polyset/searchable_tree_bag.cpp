/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   searchable_tree_bag.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 17:27:47 by fatkeski          #+#    #+#             */
/*   Updated: 2025/07/27 18:45:15 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "searchable_tree_bag.hpp"

searchable_tree_bag::searchable_tree_bag(){}
searchable_tree_bag::searchable_tree_bag(const searchable_tree_bag &source) : tree_bag(source) {}
searchable_tree_bag& searchable_tree_bag::operator=(const searchable_tree_bag &source){
        if(this != &source)
            tree_bag::operator=(source);
        return *this;
}


bool searchable_tree_bag::search(node *n, const int value){
    if(n == nullptr)
        return false;
    if(n->value == value)
        return true;
    if(value < n->value)
        search(n->l, value);
    else {
        search(n->r, value);
    }
}

bool searchable_tree_bag::has(int value){ return search(this->tree, value);}
searchable_tree_bag::~searchable_tree_bag(){}
