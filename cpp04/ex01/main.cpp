#include <cstdlib>
#include <cstdio>
#include "Animal.hpp"
#include "Cat.hpp"
#include "Dog.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"
#include "Brain.hpp"

int main(void)
{
    std::cout << "====================" << std::endl;
    std::cout << "==== ANIMAL TEST ===" << std::endl;
    std::cout << "====================\n" << std::endl;

    std::cout << "Constructing Animal" << std::endl;
    const Animal *meta[10];

    for(size_t i = 0; i < 10; i++)
    {
        if(i % 2 == 0)
            meta[i] = new Dog();
        else
            meta[i] = new Cat();
    }
    std::cout << std::endl;

    std::cout << "====================" << std::endl;
    std::cout << "==== ANIMAL TEST 2 =" << std::endl;
    std::cout << "====================\n" << std::endl;

    for(size_t i = 0; i < 10; i++)
    {
        std::cout << "Animal _type: " << meta[i]->getType() << std::endl;
        meta[i]->makeSound();
        std::cout << std::endl;
    }
    std::cout << std::endl;

    for(size_t i = 0; i < 10; i++)
        delete meta[i];
    std::cout << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "=== showing that the copy constructor create a deep copy ===" << std::endl;
    std::cout << "============================================================\n" << std::endl;

    std::cout << "Constructing Animal" << std::endl;
    
    try{
        Dog *dog = new Dog();
        std::cout << std::endl;
        dog->setIdea(0, "I am a dog");
        dog->setIdea(1, "I am a dog 2");
        dog->setIdea(2, "I am a dog 3");
        dog->setIdea(101, "some shit");
        try{
            Dog *dog2 = new Dog(*dog);
            std::cout << std::endl;
            std::cout <<"### Testing Dog ###\n" << std::endl;
            std::cout << "The " << dog->getType() << " a has the following ideas: " << std::endl;
            dog->getIdea();
            std::cout << std::endl;
            std::cout << "### Testing dog2 ###\n" << std::endl;
            dog2->getIdea();
            for(int i = 0; i != 100; i++)
                dog2->setIdea(i, "Dog!?");
            delete(dog);   
            std::cout << std::endl;
            std::cout << "The " << dog2->getType() << " a has the following ideas: " << std::endl;
            dog2->getIdea();
            std::cout << std::endl;
            std::cout << "Deconstructing" << std::endl;
            delete(dog2);
        } catch (const std::bad_alloc& e){
            std::cerr << " Memoory allocation failed: " << std::endl;
        }
    } catch (const std::bad_alloc& e){
        std::cerr << " Memoory allocation failed: " << std::endl;
    }
    return (0);
}  

