#include "mini_db.hpp"
#include <fstream>
#include <csignal>

std::string path;
std::map<std::string, std::string> db;

void handler(int signalNum){
    std::ofstream file(path.c_str());
    if(!file.is_open()){
        std::cout << "Wrong" << std::endl;
        exit(signalNum);
    }
    std::map<std::string, std::string>::const_iterator it;
    for(it = db.begin(); it != db.end(); it++)
        file << it->first << " " << it->second << std::endl;
    file.close();
    exit(signalNum);
}

void readDB()
{
    std::ifstream file(path.c_str());
    if(!file.is_open()) return;
    std::string key, value;
    while(file >> key >> value)
        db[key] = value;
}

int main(int ac, char **av)
{   
    if(ac != 3){
        std::cout << "Wrong" << std::endl;
        return 1;
    } 
    path = av[2];
    signal(SIGINT, handler);
    readDB();

	Server server(std::atoi(av[1]), db);
	return server.run();
}
