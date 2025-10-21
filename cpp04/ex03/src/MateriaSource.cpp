#include "../include/MateriaSource.hpp"

MateriaSource::MateriaSource(){
    for(int i = 0; i < 4; i++){
        materia[i] = 0;
    }
}

MateriaSource::MateriaSource(const MateriaSource &other){
    for(int i = 0; i < 4; i++){
        if(other.materia[i])
            materia[i] = other.materia[i]->clone();
        else
            materia[i] = 0;
    }
}


MateriaSource &MateriaSource::operator=(const MateriaSource &other){
    if(this != &other){
        for(int i = 0; i < 4; i++){
            if(materia[i])
                delete materia[i];
            if(other.materia[i])
                materia[i] = other.materia[i]->clone();
            else
                materia[i] = 0;
        }
    }
    return *this;
}

MateriaSource::~MateriaSource(){
    for(int i = 0; i < 4; i++){
        if(materia[i])
            delete materia[i];
    }
}

void MateriaSource::learnMateria(AMateria *m){
    for(int i = 0; i < 4; i++){
        if(!materia[i]){
            materia[i] = m;
            return;
        }
    }
}

AMateria* MateriaSource::createMateria(std::string const &type){
    for(int i = 0; i < 4; i++){
        if(materia[i] && materia[i]->getType() == type){
            return materia[i]->clone();
        }
    }
    return 0;
}


