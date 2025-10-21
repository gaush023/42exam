#include "AAnimal.hpp"

AAnimal::AAnimal() : _type("Animal")
{
    std::cout << "Animal constructor called" << std::endl;
}

AAnimal::AAnimal(const std::string &copy_name) : _type(copy_name)
{
    std::cout << "Animal constructor called" << std::endl;
}

AAnimal::~AAnimal()
{
    std::cout << "Animal destructor called" << std::endl;
}

AAnimal &AAnimal::operator=(const AAnimal &copy)
{
    std::cout << "Animal operator= called" << std::endl;
    if (this == &copy)
        return *this;
    this->_type = copy._type;
    return *this;
}

void AAnimal::makeSound(void) const
{
    std::cout << "This animal does not make a sound" << std::endl;
}

std::string AAnimal::getType(void) const
{
    return this->_type;
}

