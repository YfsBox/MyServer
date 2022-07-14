#ifndef MYSERVER_HTTPDATA_H
#define MYSERVER_HTTPDATA_H

#include <memory>

#include "HttpRequest.h"
#include "HttpReponse.h"
#include "Socket.h"
#include "Timer.h"

//代表一个已经与server建立的连接
class TimerNode;

class Httpdata {//这个数据结构究竟都是怎么用的
public:

    typedef std::shared_ptr<HttpRequest> spRequest;
    typedef std::shared_ptr<HttpReponse> spReponse;
    typedef std::shared_ptr<ClientSocket> spClient;

    Httpdata(){}


    void closeTimer();
    void setTimer(std::shared_ptr<TimerNode> tnode);
    
public:

    spRequest request_;
    spReponse reponse_;
    spClient client_;
    
private:
    std::weak_ptr<TimerNode> tnode_;
};

# endif