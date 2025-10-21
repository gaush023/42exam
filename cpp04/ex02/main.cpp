#include <cstdlib>
#include <cstdio>
#include "AAnimal.hpp"
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

    try{
        const AAnimal* dog = new Dog();
        const AAnimal* cat = new Cat();
        std::cout << std::endl;
        std::cout << "Dog type: " << dog->getType() << std::endl;
        std::cout << "Cat type: " << cat->getType() << std::endl;
        dog->makeSound();
        cat->makeSound();
        std::cout << std::endl;
        delete dog;
        delete cat;
    } catch (const std::exception &e){
        std::cerr << e.what() << std::endl;
    }
    std::cout << std::endl;

    return (0);
}  



