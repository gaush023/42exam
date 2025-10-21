#pragma once
#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include <iostream>
#include <string>

class Animal
{
    protected:
        std::string _type;
    
    public:
        
        Animal();
        Animal(const std::string &copy_name);

        virtual ~Animal();

        Animal &operator=(const Animal &copy);

        virtual void makeSound(void) const;
        std::string getType(void) const;
};

#endif // !ANIMAL_HPP

