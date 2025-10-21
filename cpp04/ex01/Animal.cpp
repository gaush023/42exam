#include "Animal.hpp"

Animal::Animal() : _type("Animal")
{
    std::cout << "Animal constructor called" << std::endl;
}

Animal::Animal(const std::string &copy_name) : _type(copy_name)
{
    std::cout << "Animal constructor called" << std::endl;
    *this = copy_name;
}

Animal::~Animal()
{
    std::cout << "Animal destructor called" << std::endl;
}

Animal &Animal::operator=(const Animal &copy)
{
    std::cout << "Animal operator= called" << std::endl;
    if (this == &copy)
        return *this;
    this->_type = copy._type;
    return *this;
}

void Animal::makeSound(void) const
{
    std::cout << "This animal does not make a sound" << std::endl;
}

std::string Animal::getType(void) const
{
    return this->_type;
}

