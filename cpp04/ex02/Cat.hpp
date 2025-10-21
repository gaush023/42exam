#pragma once
#ifndef CAT_HPP
#define CAT_HPP

#include "AAnimal.hpp"
#include "Brain.hpp"

class Cat : public AAnimal
{
    private:
        Brain *_brain;

    public:
        Cat();
        Cat(const Cat &copy);

        virtual ~Cat();
    
        Cat &operator=(const Cat &copy);

        void makeSound(void) const;
        void getIdea(void) const;
        void setIdea(size_t index, std::string idea);
};


#endif 

