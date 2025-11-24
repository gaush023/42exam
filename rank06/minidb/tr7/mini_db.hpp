#pragma once

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <string>
#include <map>
#include <sstream>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

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

        // アドレス再利用（テスト連続実行対策）
        int opt = 1;
        if (::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            throw std::runtime_error("setsockopt failed");

        std::memset(&_servaddr, 0, sizeof(_servaddr));
        _servaddr.sin_family      = AF_INET;
        _servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        _servaddr.sin_port        = htons(port);
    }

    ~Socket()
    {
        if (_sockfd != -1)
            ::close(_sockfd);
    }

    void bindAndListen()
    {
        if (::bind(_sockfd, (struct sockaddr *)&_servaddr, sizeof(_servaddr)) < 0)
            throw std::runtime_error("Socket bind failed");

        if (::listen(_sockfd, 10) < 0)
            throw std::runtime_error("Socket listen failed");
    }

    int accept(struct sockaddr_in &clientAddr)
    {
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocketFd = ::accept(_sockfd, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSocketFd < 0)
            throw std::runtime_error("Failed to accept connection");
        return clientSocketFd;
    }

    std::string pullMessage(int clientFd)
    {
        char buf[1024];
        int byte_read = recv(clientFd, buf, sizeof(buf) - 1, 0);
        if (byte_read <= 0)
            return std::string("");
        buf[byte_read] = '\0';
        return std::string(buf);
    }
};

class Server
{
private:
    Socket _listeningSocket;
    std::map<std::string, std::string> &db;
    fd_set rfds, afds;
    int max_fd;

    void handle_msg(int clientFd, std::string msg)
    {
        // 末尾の改行文字を削除 (実際の\nと、リテラルの\nの両方)
        while (!msg.empty() && (msg[msg.length() - 1] == '\n' || msg[msg.length() - 1] == '\r'))
            msg.erase(msg.length() - 1);
        // リテラルの"\n"を削除 (バックスラッシュ + n)
        while (msg.length() >= 2 && msg[msg.length() - 2] == '\\' && msg[msg.length() - 1] == 'n')
            msg.erase(msg.length() - 2);

        std::istringstream iss(msg);
        std::string command, key, value;
        iss >> command >> key >> value;

        if (command == "POST" && !value.empty())
        {
            db[key] = value;
            const char *res = "0\n";
            ::send(clientFd, res, 2, 0);
        }
        else if (command == "GET" && value.empty())
        {
            std::map<std::string, std::string>::iterator it = db.find(key);
            if (it != db.end())
            {
                std::string res = "0 " + it->second + "\n";
                ::send(clientFd, res.c_str(), res.size(), 0);
            }
            else
            {
                const char *res = "1\n";
                ::send(clientFd, res, 2, 0);
            }
        }
        else if (command == "DELETE" && value.empty())
        {
            std::map<std::string, std::string>::iterator it = db.find(key);
            if (it != db.end())
            {
                db.erase(it);
                const char *res = "0\n";
                ::send(clientFd, res, 2, 0);
            }
            else
            {
                const char *res = "1\n";
                ::send(clientFd, res, 2, 0);
            }
        }
        else
        {
            const char *res = "2\n";
            ::send(clientFd, res, 2, 0);
        }
    }

public:
    Server(int port, std::map<std::string, std::string> &database)
        : _listeningSocket(port), db(database), max_fd(0)
    {
        FD_ZERO(&afds);
    }

    int run()
    {
        try
        {
            _listeningSocket.bindAndListen();
            max_fd = _listeningSocket._sockfd;
            FD_SET(max_fd, &afds);

            // listen 成功後に ready 出力（テスター仕様）
            std::cout << "ready" << std::endl;

            while (true)
            {
                struct sockaddr_in clientAddr;
                rfds = afds;

                if (select(max_fd + 1, &rfds, NULL, NULL, NULL) < 0)
                    throw std::runtime_error("select failed");

                for (int fd = 0; fd <= max_fd; fd++)
                {
                    if (!FD_ISSET(fd, &rfds))
                        continue;

                    if (fd == _listeningSocket._sockfd)
                    {
                        // 新規接続
                        int clientFd = _listeningSocket.accept(clientAddr);
                        FD_SET(clientFd, &afds);
                        max_fd = clientFd > max_fd ? clientFd : max_fd;
                    }
                    else
                    {
                        // 既存接続からのメッセージ
                        std::string message = _listeningSocket.pullMessage(fd);
                        if (message.empty())
                        {
                            // 接続切断
                            ::close(fd);
                            FD_CLR(fd, &afds);
                            break;
                        }
                        handle_msg(fd, message);
                    }
                }
            }
            return 0;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error during server run: " << e.what() << std::endl;
            return 1;
        }
    }
};
