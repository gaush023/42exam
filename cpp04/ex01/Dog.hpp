#pragma once
#ifndef DOG_HPP
#define DOG_HPP

#include <cstdlib>
#include "Animal.hpp"
#include "Brain.hpp"

class Dog : public Animal
{
    private:
        Brain *_brain;

    public:
        Dog();
        Dog(const Dog &copy);

        virtual ~Dog();
    
        Dog &operator=(const Dog &copy);

        void makeSound(void) const;       
        void getIdea(void) const;
        void setIdea(size_t index, std::string idea);
};


#endif // !DOG_HPP
