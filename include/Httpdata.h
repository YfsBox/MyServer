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

//这里的的request和reponse类似于一个client接收或者发送到server的一个buffer
//request需要从recv的缓冲区中解析出来
//reponse先构造出reponse结构体,然后写到sent的缓冲区中

//其中定时器的作用是什么呢?当每一个连接建立时,就会有一个相应的timernode加入到manager中
//


# endif