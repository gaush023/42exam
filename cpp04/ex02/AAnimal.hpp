#pragma once
#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include <iostream>
#include <string>

class AAnimal
{
    protected:
        std::string _type;
    
    public:
        
        AAnimal(void);
        AAnimal(const std::string &copy_name);

        virtual ~AAnimal();

        AAnimal &operator=(const AAnimal &copy);

        virtual void makeSound(void) const = 0;
        std::string getType(void) const;
};

#endif // !ANIMAL_HPP

