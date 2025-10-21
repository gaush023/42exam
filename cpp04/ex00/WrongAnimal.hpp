#pragma once
#ifndef WRONGANIMAL_HPP
#define WRONGANIMAL_HPP

#include <iostream>

class WrongAnimal
{
    protected:
        std::string _type;
    
    public:
        WrongAnimal();
        WrongAnimal(const std::string &copy_name);

        ~WrongAnimal();

        WrongAnimal &operator=(const WrongAnimal &copy);

        virtual void makeSound(void) const;
        std::string getType(void) const;
};

#endif // !WRONGANIMAL_HPP
