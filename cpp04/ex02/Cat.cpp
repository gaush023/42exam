#include "Cat.hpp"

Cat::Cat() :AAnimal()
{
    this->_type = "Cat";
    std::cout << "Cat constructor called" << std::endl;
}

Cat::Cat(const Cat &copy) :AAnimal()
{
    std::cout << "Cat copy constructor called" << std::endl;
    *this = copy;
}

Cat::~Cat()
{
    std::cout << "Cat destructor called" << std::endl;
}

Cat &Cat::operator=(const Cat &copy)
{
    std::cout << "Cat operator= called" << std::endl;
    if (this == &copy)
        return *this;

    this->_type = copy._type;
    return *this;
}

void Cat::makeSound(void) const
{
    std::cout << this->getType() << " says Meow" << std::endl;
}

void Cat::getIdea(void) const
{
    for(size_t i = 0; i < 3; i++)
    {
        if(this->_brain->getIdea(i).length() > 0)
            std::cout << "Idea " << i << ": " << this->_brain->getIdea(i) << std::endl;
    }
}

void Cat::setIdea(size_t index, std::string idea)
{
    this->_brain->setIdea(index, idea);
}

