#include "Brain.hpp"

Brain::Brain()
{
    std::cout << "Brain constructor called" << std::endl;
    for(size_t i = 0; i < 100; i++)
        this->_ideas[i] = "";
}

Brain::Brain(const Brain &copy)
{
    std::cout << "Brain copy constructor called" << std::endl;
    *this = copy;
}

Brain::~Brain()
{
    std::cout << "Brain destructor called" << std::endl;
}

Brain &Brain::operator=(const Brain &copy)
{
    std::cout << "Brain assignation operator called" << std::endl;
    if(this == &copy)
        return *this;
    for(size_t i = 0; i < 100; i++)
    {
        if(copy._ideas[i].length() > 0)
            this->_ideas[i] = copy._ideas[i];
    }
    return *this;
}

const std::string Brain::getIdea(size_t index) const
{
    if(index >=100)
        return "Index out of range, please provide a valid index";
    return this->_ideas[index];
}

const std::string *Brain::getIdeaAddress(size_t index) const
{
    if(index < 100)
        return &this->_ideas[index];
    return NULL;
}

void Brain::setIdea(size_t index, std::string idea)
{
    if(index >= 100)
    {
        std::cout << "Index out of range, please provide a valid index" << std::endl;
        return;
    }
    if(index < 5)
    {
        std::cout << "Setting idea at index " << index << std::endl;
        std::cout << "Idea: " << idea << std::endl;
    }
    else if(index == 5)
        std::cout << "None of your business\n" << std::endl;
    this->_ideas[index] = idea;
}

