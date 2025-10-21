#include "Dog.hpp"

Dog::Dog() : Animal()
{
    this->_type = "Dog";
    std::cout << "Dog constructor called" << std::endl;
    try{
        this->_brain = new Brain();
    } catch (const std::bad_alloc& e){
        std::cout << "Failed to allocate memory for Dog Brain" << std::endl;
        exit(1);
    }
}

Dog::Dog(const Dog &copy) : Animal()
{
    std::cout << "Dog copy constructor called" << std::endl;
    *this = copy;
}

Dog::~Dog()
{
    delete(this->_brain);
    std::cout << "Dog destructor called" << std::endl;
}

Dog &Dog::operator=(const Dog &copy)
{
    std::cout << "Dog operator= called" << std::endl;
    if (this == &copy)
        return *this;
    this->_type = copy._type;
    try{
        this->_brain = new Brain();
    } catch (const std::bad_alloc& e){
        std::cout << "Failed to allocate memory for Dog Brain" << std::endl;
        exit(1);
    }
    return *this;
}

void Dog::makeSound(void) const
{
    std::cout << this->getType() << " says Woof" << std::endl;
}

void Dog::getIdea(void) const
{
    std::cout << "in " << this->getType()  << " getIdea" << std::endl;
    for(size_t i = 0; i < 3; i++)
    {
        if(this->_brain->getIdea(i).length() > 0)
            std::cout << "Idea " << i << ": " << this->_brain->getIdea(i) << std::endl;
        else
            std::cout << "Idea " << i << ": " << "No idea" << std::endl;
    }
}

void Dog::setIdea(size_t index, std::string idea)
{
    this->_brain->setIdea(index, idea);
}

