#pragma once

#include "AMateria.hpp"
#include "ICharacter.hpp"

class Ice : public AMateria{
    public:
        Ice();
        virtual ~Ice();
        
        AMateria* clone() const;
        void use(ICharacter &target);
};
