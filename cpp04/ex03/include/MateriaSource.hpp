#pragma once    

#include "IMateriaSource.hpp"

class MateriaSource : public IMateriaSource{
    private:
        AMateria* materia[4];

    public:
        MateriaSource();
        MateriaSource(const MateriaSource &other);
        MateriaSource &operator=(const MateriaSource &other);
        virtual ~MateriaSource();

        void learnMateria(AMateria *m);
        AMateria* createMateria(std::string const &type);
};

