//
// Created by yfs on 2022/6/26.
//
#ifndef MYSERVER_SERVER_H
#define MYSERVER_SERVER_H

#include <memory>
#include <string>
#include "Httpdata.h"
#include "Socket.h"
#include "Log.h"



class Server {//采用单例模式构建Server
public:
    //static Server *getInstance();
    Server();
    ~Server();

    int init(const std::string ip,const int port);
    void run(const int max_thread_size,const int max_request_size,const int max_event_size);
    void doRequest(std::shared_ptr<void> args);
    void getMine(std::shared_ptr<Httpdata> data);
    HttpReponse::FILE_STATUS getFile(std::shared_ptr<Httpdata> data);
private:
    std::string ip_;
    int port_; 
    std::shared_ptr<ServerSocket> serverSock;

};
#endif //MYSERVER_SERVER_H
