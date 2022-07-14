//
// Created by yfs on 2022/6/26.
//
#include "../include/Socket.h"
#include <fcntl.h>
#include <string>
#include <strings.h>

int setnonblock(int fd){
    int old_version = fcntl(fd,F_GETFL);
    int new_version = (old_version | O_NONBLOCK);
    fcntl(fd,F_SETFL,new_version);//这里当时是因为set而错的
    return old_version;
}

static void setReusePort(int fd) {//这个函数的作用有些不大明白
    int opt = 1;
    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (const void *)&opt, sizeof(opt));
}

int ClientSocket::close() {
    if(fd_ > 0){
        ::close(fd_);
    }
    return 0;
}

ServerSocket::ServerSocket(const std::string ip, const int port):
ip_(ip),port_(port),listen_fd_(-1){
    bzero(&addr_,sizeof (addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port_);
    ::inet_pton(AF_INET,ip.c_str(),&addr_.sin_addr);

    int ret = socket(AF_INET,SOCK_STREAM,0);
    if(ret < 0) {
        //std::cout<<"Create listen socket error\n";
        LOGERROR_F("Create listen socket error");
        exit(0);
    }
    listen_fd_ = ret;

    setReusePort(listen_fd_);
    setnonblock(listen_fd_);
    //std::cout<<"the server is no block\n";
}

ServerSocket::~ServerSocket() {
    close();
}

int ServerSocket::bind() {
    int ret = ::bind(listen_fd_,(struct sockaddr*) &addr_,sizeof (addr_));
    if(ret < 0){
        //std::cout<<"The ServerSocket bind error\n";
        LOGERROR_F("The ServerSocket bind error");
        return -1;
    }
    return 0;
}

int ServerSocket::listen() {
    int ret = ::listen(listen_fd_,1024);
    if(ret < 0){
        //std::cout<<"The ServerSocket listen error\n";
        LOGERROR_F("The ServerSocket listen error");
        return -1;
    }
    return 0;
}

int ServerSocket::accept(ClientSocket &client) {
    //int ret = ::accept(listen_fd_,NULL,NULL);
    //std::cout<<"begin accept in server socket\n"; 
    int ret = ::accept4(listen_fd_,NULL,NULL,SOCK_NONBLOCK);
    if(ret < 0){
        //std::cout<<"The ServerSocket accpet error\n";
        LOGERROR_F("The ServerSocket accpet error");
        return -1;
    }
    client.setfd(ret);
    return ret;
}

int ServerSocket::close() {
    if(listen_fd_ > 0){
        ::close(listen_fd_);
        listen_fd_ = -1;
    }
    return 0;
}


