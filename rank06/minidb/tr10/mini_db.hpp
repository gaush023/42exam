#pragma once

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <signal.h>
#include <sys/epoll.h>
#include <map>
#include <string>
#include <sstream>
#include <errno.h>

class Socket
{
private:
	struct sockaddr_in _servaddr;

public:
	int _sockfd;
	Socket(int port) :
			_sockfd(socket(AF_INET, SOCK_STREAM, 0))
		{
			if(_sockfd == -1){
				throw std::runtime_error("Socket creation failed");
			}
			int opt = 1;
			if(::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
				throw std::runtime_error("Socket creation failed");
			memset(&_servaddr, 0, sizeof(_servaddr));
			_servaddr.sin_family = AF_INET;
			_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
			_servaddr.sin_port = htons(port);
		}

	~Socket()
		{
			if (_sockfd != -1)
			{
				close(_sockfd);
			}
		}
	void bindAndListen()
		{
			if(bind(_sockfd, (struct sockaddr *)&_servaddr, sizeof(_servaddr)) < 0)
			{
				throw std::runtime_error("Socket bind failed");
			}

			if (listen(_sockfd, 10) < 0)
			{
				throw std::runtime_error("Socket listen failed");
			}
		}
	int accept(struct sockaddr_in& clientAddr)
		{
			socklen_t clientLen = sizeof(clientAddr);
			int clientSocketFd = ::accept(_sockfd, (struct sockaddr*)&clientAddr, &clientLen);
			if(clientSocketFd < 0)
			{
				throw std::runtime_error("Failed to accept connection");
			}
			return clientSocketFd;
		}
	std::string pullMessage(int clientfd)
	{
		char buf[1024];
		int r = recv(clientfd, buf, sizeof(buf)-1, 0);
		if(r <= 0)
		return std::string("");
		buf[r] = '\0'; 
		return std::string(buf); 
        }
};  

class Server
{
private: 
	Socket _listeningSocket;
	std::map<std::string, std::string> &db;
       	std::map<int, std::string> pending_writes;
	int epoll_fd;
	
	void handle_msg(int clientfd, std::string msg){
		while(!msg.empty() && (msg[msg.length() - 1] == '\n' || msg[msg.length() - 1] == '\r'))
			msg.erase(msg.length() - 1);
		while(msg.length() >= 2 && msg[msg.length() - 2] == '\\' && msg[msg.length() - 1] == 'n')
			msg.erase(msg.length() - 2, 2);
		std::istringstream iss(msg);
		std::string command, key, value;
		iss >> command >> key;
		std::getline(iss, value);
		if(!value.empty() && value[0] == ' ')
			value.erase(0, 1);
		std::string response;
		if(command == "POST" && !value.empty()){
			db[key] = value;
			response = "0\n";
		} else if(command == "GET" && value.empty()){
			std::map<std::string, std::string>::iterator it = db.find(key);
			if(it != db.end())
				response = "0 " + it->second + "\n";
			else
				response = "1\n";
		} else if(command == "DELETE" && value.empty()){
			std::map<std::string, std::string>::iterator it = db.find(key);
			if(it != db.end()){
				db.erase(it);
				response = "0\n";
			} else
				response = "1\n";
		} else
				response = "2\n";
		pending_writes[clientfd] = response;
		struct epoll_event ev;
		ev.events = EPOLLOUT | EPOLLET;
		ev.data.fd = clientfd;
		epoll_ctl(epoll_fd, EPOLL_CTL_MOD, clientfd, &ev);
	}
		
public:
	Server(int port, std::map<std::string, std::string> &database) :
	_listeningSocket(port), db(database), epoll_fd(-1)
	{
		epoll_fd = epoll_create1(0);
		if(epoll_fd == -1)
			throw std::runtime_error("Failed to accept connection");
	}
	~Server(){
		if(epoll_fd != -1)
			close(epoll_fd);	
	}
	int run()
		{
			try
			{
				_listeningSocket.bindAndListen();
				struct epoll_event ev;
				ev.events = EPOLLIN;
				ev.data.fd = _listeningSocket._sockfd;
				if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1)
					throw std::runtime_error("Failed to accept connection");
				std::cout << "ready" << std::endl;
				struct epoll_event events[1024];
				while(true){
					int nfds = epoll_wait(epoll_fd, events, 1024, -1);
					if(nfds == -1){
						if(errno == EINTR)
							continue;
						throw std::runtime_error("Failed to accept connection");
					}
				for(int i = 0; i < nfds; i++){
					int fd = events[i].data.fd;
					if(fd == _listeningSocket._sockfd){
						struct sockaddr_in clientAddr;
						int clientfd = _listeningSocket.accept(clientAddr);
						struct epoll_event cev;
						cev.events = EPOLLIN | EPOLLET;
						cev.data.fd = clientfd;
						if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientfd, &cev) == -1){
							::close(fd);
							continue;		
						}
					} else if(events[i].events & EPOLLIN){
						std::string msg = _listeningSocket.pullMessage(fd);
						if(msg.empty()){
							epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
							::close(fd);	
							continue;
						}
						handle_msg(fd, msg);
					} else if(events[i].events & EPOLLOUT){
						std::map<int, std::string>::iterator it = pending_writes.find(fd);
						if(it != pending_writes.end()){
							const std::string &response = it->second;
							ssize_t sent = send(fd, response.c_str(), response.size(), 0);
							if(sent > 0){
								epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
								pending_writes.erase(it);
								::close(fd);
							}
						}
					}
						
					}
				}
				return 0; //Success
			}
			catch(const std::exception& e)
			{
				std::cerr << "Error during server run: " << e.what() << std::endl;
				return 1; // Return error code if server fails to start
			}
		}
};

