#pragma once

#include "AMateria.hpp"
#include "ICharacter.hpp"


class Cure : public AMateria{
    public:
        Cure();
        virtual ~Cure();
        
        AMateria *clone() const;
        void use(ICharacter &target);
};


