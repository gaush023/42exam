#include "WrongCat.hpp"

WrongCat::WrongCat() : WrongAnimal()
{
    this->_type = "WronCat";
    std::cout << "WronCat constructor called" << std::endl;
}

WrongCat::WrongCat(const WrongCat &copy) : WrongAnimal()
{
    std::cout << "WrongCat copy constructor called" << std::endl;
    *this = copy;
}

WrongCat::~WrongCat()
{
    std::cout << "WrongCat destructor called" << std::endl;
}


WrongCat &WrongCat::operator=(const WrongCat &copy)
{
    std::cout << "WrongCat operator= called" << std::endl;
    if (this == &copy)
        return *this;

    this->_type = copy._type;
    return *this;
}


void WrongCat::makeSound(void) const
{
    std::cout << this->getType() << " says Meow" << std::endl;
}


