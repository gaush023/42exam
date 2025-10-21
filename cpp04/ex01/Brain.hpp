#pragma once
#ifndef BRAIN_HPP
#define BRAIN_HPP


#include <iostream>

class Brain
{
    private:
        std::string _ideas[100];

    public:
        Brain();
        Brain(const Brain &copy);

        ~Brain();

        Brain &operator=(const Brain &copy);
        
        const std::string getIdea(size_t index) const;
        const std::string  *getIdeaAddress(size_t index) const;

        void setIdea(size_t index, std::string idea);
};

#endif

