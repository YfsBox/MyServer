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

private:
    int epoll_fd_;
    int listen_fd_;
    int max_event_size_;
    epoll_event *events_;
    std::unordered_map<int,std::shared_ptr<Httpdata> > httpmaps_;

    static TimerManager timermanager;

    int handleConnection(ServerSocket &server);

};

#endif //MYSERVER_EPOLL_H
