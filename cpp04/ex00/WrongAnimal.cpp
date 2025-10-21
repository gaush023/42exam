#include "WrongAnimal.hpp"

WrongAnimal::WrongAnimal():_type("wrong_default")
{
    std::cout << "WrongAnimal constructor called" << std::endl;
}

WrongAnimal::WrongAnimal(const std::string &copy_name)
{
    std::cout << "WrongAnimal constructor called" << std::endl;
    *this = copy_name;
}

WrongAnimal::~WrongAnimal()
{
    std::cout << "WrongAnimal destructor called" << std::endl;
}

WrongAnimal &WrongAnimal::operator=(const WrongAnimal &copy)
{
    std::cout << "WrongAnimal operator= called" << std::endl;
    if (this == &copy)
        return *this;
    this->_type = copy._type;
    return *this;
}

void WrongAnimal::makeSound(void) const
{
    std::cout << this->getType() << " syas: **WrongAnimal sound**" << std::endl;
}

std::string WrongAnimal::getType(void) const
{
    return this->_type;
}


