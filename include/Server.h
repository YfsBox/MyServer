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


//这个类主要就是维护了一个监听的socket,这个socket可以视为整个server的门户
//其实还有线程池，epoll也是关键的部分,这些部分在run中就被初始化了

class Server {//采用单例模式构建Server
public:
    //static Server *getInstance();
    Server();
    ~Server();

    int init(const std::string ip,const int port); //init处理了关于socket的部分
    void run(const int max_thread_size,const int max_request_size,const int max_event_size);
    void doRequest(std::shared_ptr<void> args);
    void getMine(std::shared_ptr<Httpdata> data);//其中主要根据uri请求文件的后缀来判断mine类型
    HttpReponse::FILE_STATUS getFile(std::shared_ptr<Httpdata> data);

    //其中get有关的函数用来解析request生成需要用在reponse的东西比如mine，file等，这种功能被汇总在doRequest,并且还会有send
private:
    std::string ip_;
    int port_; 
    std::shared_ptr<ServerSocket> serverSock;

};
#endif //MYSERVER_SERVER_H
