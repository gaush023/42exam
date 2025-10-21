#include "../include/AMateria.hpp"
#include "../include/ICharacter.hpp"


AMateria::AMateria(std::string const &type) : type(type) {}
AMateria::~AMateria() {}

std::string const &AMateria::getType() const{
    return type;
}

void AMateria::use(ICharacter &target){
    (void)target;
    std::cout << "* generic materia action *" << std::endl;
}


