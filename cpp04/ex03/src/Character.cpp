#include "../include/Character.hpp"
#include <iostream>

Character::Character(std::string const &name) : name(name) {
    for(int i = 0; i < 4; i++){
        inventory[i] = 0;
    }
}

Character::Character(const Character &other) : name(other.name) {
    for(int i = 0; i < 4; i++){
        if(other.inventory[i])
            inventory[i] = other.inventory[i]->clone();
        else 
            inventory[i] = 0;
    }
}

Character &Character::operator=(const Character &other){
    if(this != &other){
        name = other.name;
        for(int i = 0; i < 4; i++){
            if(inventory[i])
                delete inventory[i];
            if(other.inventory[i])
                inventory[i] = other.inventory[i]->clone();
            else
                inventory[i] = 0;
        }
    }
    return *this;
}


Character::~Character(){
    for(int i = 0; i < 4; i++){
        if(inventory[i])
            delete inventory[i];
    }
}

std::string const &Character::getName() const{
    return name;
}

void Character::equip(AMateria *m){
    for(int i = 0; i < 4; i++){
        if(!inventory[i]){
            inventory[i] = m;
            return;
        }
    }
}

void Character::unequip(int idx){
    if(idx < 0 || idx >= 4)
        return;
    inventory[idx] = 0;
}

void Character::use(int idx, ICharacter &target){
    if(idx < 0 || idx >= 4 || !inventory[idx])
        return;
    inventory[idx]->use(target);
}

