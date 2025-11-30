#pragma once

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <string>
#include <map>
#include <sstream>
#include <cerrno>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

class Socket
{
private:
    struct sockaddr_in _servaddr;

public:
    int _sockfd;

    Socket(int port)
        : _sockfd(::socket(AF_INET, SOCK_STREAM, 0))
    {
        if (_sockfd == -1)
            throw std::runtime_error("Socket creation failed");

        int opt = 1;
        if (::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            throw std::runtime_error("setsockopt failed");

        memset(&_servaddr, 0, sizeof(_servaddr));
        _servaddr.sin_family      = AF_INET;
        _servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        _servaddr.sin_port        = htons(port);

        // listening socket も nonblock にしてOK
        fcntl(_sockfd, F_SETFL, O_NONBLOCK);
    }

    ~Socket()
    {
        if (_sockfd != -1)
            close(_sockfd);
    }

    void bindAndListen()
    {
        if (bind(_sockfd, (struct sockaddr *)&_servaddr, sizeof(_servaddr)) < 0)
            throw std::runtime_error("Socket bind failed");

        if (listen(_sockfd, 10) < 0)
            throw std::runtime_error("Socket listen failed");
    }

    int acceptClient()
    {
        sockaddr_in clientAddr;
        socklen_t len = sizeof(clientAddr);

        int cfd = ::accept(_sockfd, (struct sockaddr *)&clientAddr, &len);
        if (cfd < 0)
            return -1;
        fcntl(cfd, F_SETFL, O_NONBLOCK);
        return cfd;
    }

    std::string recvMsg(int fd)
    {
      std::string result;
      char buf[1024];

      while (true)
      {
          int n = recv(fd, buf, sizeof(buf) - 1, 0);

          if (n > 0)
          {
              buf[n] = 0;
              result += buf;
          }
          else if (n == -1 && errno == EAGAIN)
          {
              break;
          }
          else
          {
              return "";
          }
      }
      return result;
    }
};

// ===========================================================================

class Server
{
private:
    Socket sock;
    std::map<std::string, std::string> &db;
    int epfd;

    // fd -> 送信予定バッファ
    std::map<int, std::string> writeBuf;

    void handleMsg(int fd, std::string msg)
    {
        // remove trailing \n
        while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r'))
            msg.pop_back();

        std::istringstream iss(msg);
        std::string cmd, key, val;
        iss >> cmd >> key >> val;

        std::string resp;

        if (cmd == "POST" && !val.empty())
        {
            db[key] = val;
            resp = "0\n";
        }
        else if (cmd == "GET" && val.empty())
        {
            if (db.count(key))
                resp = "0 " + db[key] + "\n";
            else
                resp = "1\n";
        }
        else if (cmd == "DELETE" && val.empty())
        {
            if (db.count(key))
            {
                db.erase(key);
                resp = "0\n";
            }
            else
                resp = "1\n";
        }
        else
        {
            resp = "2\n";
        }

        writeBuf[fd] = resp;

        struct epoll_event ev;
        ev.events = EPOLLOUT | EPOLLIN; // OUT を有効化
        ev.data.fd = fd;
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
    }

public:
    Server(int port, std::map<std::string, std::string> &database)
        : sock(port), db(database)
    {
        epfd = epoll_create1(0);
        if (epfd < 0)
            throw std::runtime_error("epoll_create1 failed");
    }

    int run()
    {
        sock.bindAndListen();

        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = sock._sockfd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, sock._sockfd, &ev);

        std::cout << "ready" << std::endl;

        struct epoll_event events[1024];

        while (true)
        {
            int n = epoll_wait(epfd, events, 1024, -1);
            if (n < 0)
            {
                if (errno == EINTR)
                    continue;
                throw std::runtime_error("epoll_wait failed");
            }

            for (int i = 0; i < n; i++)
            {
                int fd = events[i].data.fd;

                // --- accept
                if (fd == sock._sockfd)
                {
                    int cfd;
                    while ((cfd = sock.acceptClient()) != -1)
                    {
                        struct epoll_event cev;
                        cev.events = EPOLLIN;
                        cev.data.fd = cfd;
                        epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &cev);
                    }
                    continue;
                }

                // --- readable
                if (events[i].events & EPOLLIN)
                {
                    std::string msg = sock.recvMsg(fd);
                    if (msg.empty())
                    {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                        writeBuf.erase(fd);
                        close(fd);
                        continue;
                    }
                    handleMsg(fd, msg);
                }

                // --- writable
                if (events[i].events & EPOLLOUT)
                {
                    auto it = writeBuf.find(fd);
                    if (it == writeBuf.end())
                        continue;

                    std::string &buf = it->second;

                    ssize_t sent = send(fd, buf.c_str(), buf.size(), 0);
                    if (sent == -1)
                    {
                        if (errno == EAGAIN)
                            continue; // 次のEPOLLOUTを待つ
                        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                        writeBuf.erase(fd);
                        close(fd);
                        continue;
                    }
                    if (sent < (ssize_t)buf.size())
                    {
                        buf.erase(0, sent); // 残りを保持
                        continue; // 次の EPOLLOUT
                    }
                    writeBuf.erase(fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                    close(fd);
                }
            }
        }

        return 0;
    }
};
