#include "mini_db.hpp"
#include <iostream>
#include <map>
#include <fstream>
#include <csignal>
#include <cstdlib>

std::string path;
std::map<std::string, std::string> db;

void handler(int signalName){
  std::ofstream file(path.c_str());
  if(!file.is_open()){
    std::cout << "Invalid file path" << std::endl;
    exit(signalName);
  }
  std::map<std::string, std::string>::const_iterator it;
  for(it = db.begin(); it != db.end(); ++it)
    file << it->first << " " << it->second << std::endl;    
  file.close();
  exit(signalName);
}

void readDB(){
  std::ifstream file(path.c_str());
  if(!file.is_open())
    return;
  std::string key, value;
  while(file >> key >> value)
    db[key] = value;
}

int main(int ac, char **av){
  if(ac != 3){
    std::cout << "Worng number" << std::endl;
    return 1;
  }

  signal(SIGINT, handler);
  path = av[2];
  readDB();
    
  server server(std::atoi(av[1]), db);
  server.run();
  
  return 0;
}
