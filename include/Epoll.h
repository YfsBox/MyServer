//
// Created by yfs on 2022/6/26.
//

#ifndef MYSERVER_EPOLL_H
#define MYSERVER_EPOLL_H

#include <sys/epoll.h>
#include <unordered_map>
#include <memory>
#include <vector>
#include "Socket.h"
#include "Httpdata.h"

//其中有注册epoll_fd的操作，也有加入到httpmap的操作
//还有追加到注册器的操作
class Epoll {
public:
    Epoll(ServerSocket &server, const int max_event_size);

    ~Epoll();

    int addfd(const int fd, const uint32_t event,std::shared_ptr<Httpdata> data);

    int modfd(const int fd, const uint32_t event,std::shared_ptr<Httpdata> data);

    int delfd(const int fd, const uint32_t event,std::shared_ptr<Httpdata> data);

    std::vector<std::shared_ptr<Httpdata> > epoll(ServerSocket &server, const int timeout);


    static __uint32_t DEFALUT_EVENT;
    static int MAX_EVENT_SIZE;

    static TimerManager timermanager;
private:
    int epoll_fd_;
    int listen_fd_;
    int max_event_size_;
    epoll_event *events_; //表示监听的已经就绪的事件,从事件中可以知道其中的fd和data
    std::unordered_map<int,std::shared_ptr<Httpdata> > httpmaps_;//这个映射是根据fd到data的
    //可以说是一个全局性的管理所有client连接的这么一个数据结构



    int handleConnection(ServerSocket &server);

};

#endif //MYSERVER_EPOLL_H
