#pragma once

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <map>
#include <sstream>

class ft_socket{
  private:  
    struct sockaddr_in _servaddr;
    
  public:
    int _sockfd;
    
    ft_socket(int port) : _sockfd(::socket(AF_INET, SOCK_STREAM, 0)){
      if(_sockfd < 0)
        throw std::runtime_error("socket() failed");
      std::memset(&_servaddr, 0, sizeof(_servaddr));
      _servaddr.sin_family = AF_INET;
      _servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
      _servaddr.sin_port = htons(port);
    }
    
    ~ft_socket(){if(_sockfd != -1) close(_sockfd);}
    
    void bindAndListen(){
      if(bind(_sockfd, (struct sockaddr*)&_servaddr, sizeof(_servaddr)) < 0) throw std::runtime_error("bind() failed");
      if(listen(_sockfd, 10) < 0) throw std::runtime_error("listen() failed");
    }
    
    int acceptClient(struct sockaddr_in &clientAddr){
      socklen_t len = sizeof(clientAddr);
      int clientFd = ::accept(_sockfd, (struct sockaddr *)&clientAddr, &len);
      if(clientFd < 0)
        throw std::runtime_error("accept() failed");
      return clientFd;
    }
    
    std::string pullMessage(int clientFd){
      char buf[1024];
      int r = recv(clientFd, buf, 1000, 0);
      if(r <= 0)
        return "";
      buf[r] = '\0';
      return std::string(buf);
    }
};

class server{
  private:
    ft_socket _listeningSocket;
    std::map<std::string, std::string> &db;
    
    fd_set afds;
    fd_set rfds;
    int max_fd;

  public:
    server(int port, std::map<std::string, std::string> &databse) :
    _listeningSocket(port), db(databse), max_fd(0){  
      FD_ZERO(&afds);  
    }
    
    void handle_msg(int clientFd, const std::string &msg){
      std::istringstream iss(msg);
      std::string command, key, value;
      iss >> command >> key;
      std::getline(iss, value);
      if(!value.empty() && value[0] == ' ') value.erase(0, 1);
      if(command == "POST" && !value.empty()){
        db[key] = value;
        send(clientFd, "0\n", 2, 0);
      } else if(command == "GET" && value.empty()){
        if(db.find(key) != db.end()){
          std::string res = "0 " + db[key] + "\n";
          send(clientFd, res.c_str(), res.size(), 0); 
        } else {
          send(clientFd, "1\n", 2, 0); 
        }
      } else if(command == "DELETE" && value.empty()){
        if(db.find(key) != db.end()){
          db.erase(key);
          send(clientFd, "0\n", 2, 0);
        } else{
          send(clientFd, "1\n", 2, 0);
        }
      } else{
        send(clientFd, "2\n", 2, 0); 
      }
    }
  
    int run(){
      try{
        _listeningSocket.bindAndListen();
        FD_ZERO(&afds);
        FD_SET(_listeningSocket._sockfd, &afds);
        max_fd = _listeningSocket._sockfd;
        std::cout << "ready" << std::endl;
        
        while(true){
          rfds = afds;
          if(select(max_fd + 1, &rfds, NULL, NULL, NULL) < 0)
            throw std::runtime_error("select() failed");
          for(int fd = 0; fd <= max_fd; fd++){
            if(!FD_ISSET(fd, &rfds))
              continue;
            if(fd == _listeningSocket._sockfd){  
              struct sockaddr_in clientAddr;
              int clientFd = _listeningSocket.acceptClient(clientAddr);
              FD_SET(clientFd, &afds);
              if(clientFd > max_fd)
                max_fd = clientFd;
            } else {
              std::string msg = _listeningSocket.pullMessage(fd);
              if(msg.empty()){
                close(fd);
                FD_CLR(fd, &afds);
                  
                if(fd == max_fd){
                  while(max_fd >= 0 && !FD_ISSET(max_fd, &afds))
                    max_fd--;
                  continue;
                }
              }
                
              std::istringstream iss(msg);
              std::string line;
              while(std::getline(iss, line)){
                if(!line.empty() && line[line.length() - 1] == '\r')
                  line.erase(line.length() - 1);
                if(!line.empty()) handle_msg(fd, line);
              }
            } 
          }
        }
      }
      catch (const std::exception &e){
        std::cerr << "server error: " << e.what() << std::endl;
        return 1;
      }
      return 0; 
    }
};

