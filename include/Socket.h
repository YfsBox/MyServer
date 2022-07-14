//
// Created by 杨丰硕 on 2022/6/26.
//

#ifndef MYSERVER_SOCKET_H
#define MYSERVER_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include "Log.h"

int setnonblock(int fd);

class ClientSocket {
public:
    ClientSocket():fd_(-1) {}
    ~ClientSocket() {}
    //void setfd(const int fd) { fd_ = fd; }
    int close();
    int getfd() const { return fd_; }
    void setfd(const int fd) { fd_ = fd;}
private:
    int fd_;
};

class ServerSocket {
public:
    ServerSocket(const std::string ip = "127.0.0.1",const int port = 80);
    ~ServerSocket();

    int bind();
    int listen();
    int accept(ClientSocket &client);
    int close();

    int getListenfd() const { return listen_fd_; }

private:
    std::string ip_;
    int port_;
    int listen_fd_;
    sockaddr_in addr_;
};
#endif //MYSERVER_SOCKET_H
