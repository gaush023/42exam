#pragma once
#ifndef WRONGCAT_HPP
#define WRONGCAT_HPP

#include "WrongAnimal.hpp"

class WrongCat : public WrongAnimal
{
    private:
    
    public:
        WrongCat();
        WrongCat(const WrongCat &copy);

        ~WrongCat();
    
        WrongCat &operator=(const WrongCat &copy);

        void makeSound(void) const;
};


#endif // !WRONGCAT_HPP

