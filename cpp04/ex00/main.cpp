#include "Animal.hpp"
#include "Cat.hpp"
#include "Dog.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"

int main(void)
{
    std::cout << "----- Animal tests -----" << std::endl;
    std::cout << "----- Constructing -----" << std::endl;
    try{
        const Animal *meta = new Animal();
        std::cout << std::endl;
        std::cout << "Animal type: " << meta->getType() << std::endl;
        meta->makeSound();
        std::cout << std::endl;
        std::cout << "----- Destructing -----" << std::endl;
        delete meta;
    } catch (const std::exception &e){
        std::cerr << e.what() << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "----- Cat tests -----" << std::endl;  
    std::cout << "----- Constructing -----" << std::endl;
    try{
        const Cat *cat = new Cat();
        std::cout << std::endl;
        std::cout << "Cat type: " << cat->getType() << std::endl;
        cat->makeSound();
        std::cout << std::endl;
        std::cout << "----- Destructing -----" << std::endl;
        delete cat;
    } catch (const std::exception &e){
        std::cerr << e.what() << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "----- Dog tests -----" << std::endl;
    std::cout << "----- Constructing -----" << std::endl;
    try{
        const Dog *dog = new Dog();
        std::cout << std::endl;
        std::cout << "Dog type: " << dog->getType() << std::endl;
        dog->makeSound();
        std::cout << std::endl;
        std::cout << "----- Destructing -----" << std::endl;
        delete dog;
    } catch (const std::exception &e){
        std::cerr << e.what() << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "----- WrongAnimal tests -----" << std::endl;
    std::cout << "----- Constructing -----" << std::endl;
    try{
        const WrongAnimal *wrong_animal = new WrongAnimal();
        std::cout << std::endl;
        std::cout << "WrongAnimal type: " << wrong_animal->getType() << std::endl;
        wrong_animal->makeSound();
        std::cout << std::endl;
        std::cout << "----- Destructing -----" << std::endl;
        delete wrong_animal;
    } catch (const std::exception &e){
        std::cerr << e.what() << std::endl;
    }
    std::cout << std::endl;

    std::cout << "----- WrongCat tests -----" << std::endl;
    std::cout << "----- Constructing -----" << std::endl;
    try{
        const WrongCat *wrong_cat = new WrongCat(); 
        std::cout << std::endl;
        std::cout << "WrongCat type: " << wrong_cat->getType() << std::endl;
        wrong_cat->makeSound();
        std::cout << std::endl;
        std::cout << "----- Destructing -----" << std::endl;
        delete wrong_cat;
    } catch (const std::exception &e){
        std::cerr << e.what() << std::endl;
    }
    std::cout << std::endl;
    
    return 0;

}   


