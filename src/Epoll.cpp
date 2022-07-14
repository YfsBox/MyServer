#include "../include/Epoll.h"
#include "../include/Log.h"
#include <iostream>
#include <memory>


__uint32_t Epoll::DEFALUT_EVENT = (EPOLLIN | EPOLLET | EPOLLONESHOT);
int Epoll::MAX_EVENT_SIZE = 10000;
TimerManager Epoll::timermanager;
//主要在于分配数组和创建fd
Epoll::Epoll(ServerSocket &server, const int max_event_size) :
        epoll_fd_(-1), listen_fd_(-1), max_event_size_(max_event_size), events_(nullptr) {
    int listen_fd = server.getListenfd();
    if (listen_fd < 0) {
        //std::cout<<"The Epoll listen_fd_ error\n";
        LOGERROR_F("The Epoll listen_fd_ error");
        exit(0);
    }
    listen_fd_ = listen_fd;

    if (max_event_size_ <= 0 || max_event_size_ > MAX_EVENT_SIZE) {
        max_event_size_ = MAX_EVENT_SIZE;
    }

    events_ = new epoll_event[max_event_size_];
    int ret = epoll_create(max_event_size_);

    if (ret < 0) {
        //std::cout<< "The Epoll epoll_fd_ error\n";
        LOGERROR_F("The Epoll epoll_fd_ error");
        exit(0);
    }
    epoll_fd_ = ret;
}

Epoll::~Epoll() {
    if (events_ != nullptr) {
        delete[]events_;
    }
}
//这几个函数主要是对与epoll_ctl的封装
int Epoll::addfd(const int fd, const uint32_t events, std::shared_ptr <Httpdata> data) {
    epoll_event event;
    //event.events = events;
    event.events = (EPOLLIN | EPOLLET);
    event.data.fd = fd;
    httpmaps_[fd] = data;
    int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
    if (ret < 0) {
        //std::cout<<"The fd add to epoll error\n";
        httpmaps_[fd].reset();
        LOGERROR_F("The fd add to epoll error");
        return -1;
    }
    return 0;
}

int Epoll::modfd(const int fd, const uint32_t events, std::shared_ptr <Httpdata> data) {
    epoll_event event;
    //event.events = events;
    event.events = (EPOLLIN | EPOLLET);
    event.data.fd = fd;
    httpmaps_[fd] = data;
    int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &event);
    if (ret < 0) {
        //std::cout<<"The fd mod to epoll error\n";
        httpmaps_[fd].reset();
        LOGERROR_F("The fd mod to epoll error");
        return -1;
    }

    return 0;
}

int Epoll::delfd(const int fd, const uint32_t events, std::shared_ptr <Httpdata> data) {
    epoll_event event;
    //event.events = events;
    event.events = (EPOLLIN | EPOLLET);
    event.data.fd = fd;
    int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &event);
    if (ret < 0) {
        //std::cout<<"The fd del to epoll error\n";
        LOGERROR_F("The fd del to epoll error");
        return -1;
    }
    auto it = httpmaps_.find(fd);
    if (it != httpmaps_.end()) {
        httpmaps_.erase(it);
    }
    return 0;
}
//处理listen_fd可以看作是对于accept的封装
int Epoll::handleConnection(ServerSocket &server) {
    std::cout<<"Begin cope with connection\n";
    std::shared_ptr <ClientSocket> client(new ClientSocket);
    //是不是这里的accept是有阻塞问题的???
    while (server.accept(*client) > 0) {//无论是client还是listen_fd的处理都应该是非阻塞的
        //std::cout<<"The loop in Epoll::handleConnection....\n";
        //如果接受成功
        //在下面的处理中,就是构造一个client_socket结构体并且还有data加入到map中还有addfd
        int client_fd = client->getfd();
        int nb_ret = setnonblock(client_fd);
        if (nb_ret < 0) {
            //std::cout<<"The nonblock of client in epoll is error\n";
            LOGERROR_F("The nonblock of client in epoll is error");
            continue;
        }
        //构造相应的httpdata
        std::shared_ptr <Httpdata> data(new Httpdata());
        data->request_ = std::shared_ptr<HttpRequest>(new HttpRequest());
        data->reponse_ = std::shared_ptr<HttpReponse>(new HttpReponse());

        std::shared_ptr <ClientSocket> data_client(new ClientSocket());
        data_client.swap(client);
        data->client_ = data_client;

        int add_ret = addfd(client_fd, EPOLLIN | EPOLLET, data);
        if (add_ret < 0) {
            //std::cout<<"The client fd add to events error\n";
            LOGERROR_F("The client fd add to events error");
            continue;
        }

        int add_timer_ret = timermanager.addTimerNode(data, TimerManager::DEFAULT_TIMEOUT);//此data加入计时器
        if (add_timer_ret < 0) {
            LOGERROR_F("The timer node add to timer manager error");
            continue;
        }
        //LOGOK_F("The add a connect client OK!");
        LOGDEBUG_C("The new connection fd is %d\n,and the map's size is %d\n,the timer'size is %d now", client_fd,
                httpmaps_.size(), timermanager.getSize());
    }
    //std::cout<<"handle connection finish\n";
    return 0;
}
//可以看作是对epoll_wait的封装,调用一次epoll_wait,并且对其结果进行处理
//最终返回的是结果,也就是就绪事件对应的httpdata项
std::vector <std::shared_ptr<Httpdata>> Epoll::epoll(ServerSocket &server, const int timeout) { //默认情况下是-1,也就是无限制地阻塞的
    std::vector <std::shared_ptr<Httpdata>> result;
    int ret = epoll_wait(epoll_fd_, events_, max_event_size_, timeout);
    if (ret < 0) {
        //std::cout<<"Epoll wait error\n";
        LOGERROR_F("Epoll wait error");
        return result;
    }

    int event_cnt = ret;
    int listen_fd = server.getListenfd();

    for (int i = 0; i < event_cnt; i++) {
        int tmp_fd = events_[i].data.fd;
        if (tmp_fd == listen_fd) {//此分支说明此时有连接到来,所以需要处理连接
            LOGOK_F("The epoll cope with server listen_fd ok!");
            int ret = handleConnection(server);//需要注意的是handleConnnection肯定不能是阻塞的
            if (ret < 0) {
                //std::cout<<"The handle Connection error in epoll\n";
                LOGERROR_F("The handle Connection error in epoll");
                continue;
            }
        } else {//对应的则是一般的用户进程所需要任务读/写就绪.
            //std::cout<<"The epoll is handing client\n";
            //LOGOK_F("The epoll is handing client");
            //首先排除一些失败的事件
            if ((events_[i].events & EPOLLERR) || (events_[i].events & EPOLLRDHUP) || (events_[i].events & EPOLLHUP)) {
                auto it = httpmaps_.find(tmp_fd);
                if (it != httpmaps_.end()) {
                    // 将HttpData节点和TimerNode的关联分开，这样HttpData会立即析构，在析构函数内关闭文件描述符等资源
                    it->second->closeTimer();//当有一个时间节点关闭时,总时间管理器上会讲这个节点弹出
                }
                continue;
            }
            auto it = httpmaps_.find(tmp_fd);//检查要进行读写的这个client是否处于建立的连接中
            if (it != httpmaps_.end()) {
                if (events_[i].events & EPOLLIN || events_[i].events & EPOLLPRI) {//有新的请求或者是带外数据的,在这里并没有检查可写事件
                    //也就是说sent是没有用的
                    result.push_back(it->second);//加入到返回的结果中
                    it->second->closeTimer();
                    httpmaps_.erase(it);//这里体现的逻辑是，处理好了就关闭,每次请求资源都要建立连接
                    //但此时还并没有完全关闭,只是移除了相应的数据部分,比如说定时器和map,还没有关闭client连接
                    LOGDEBUG_C("The map'size is %d,the result's size is %d\n",httpmaps_.size(),result.size());
                }
            } else {
                LOGERROR_C("can't find the client fd in map");
                ::close(tmp_fd); //如果没有就关闭,说明这个client socket当作无效的
                continue;
            }
        }
    }
    return result;
}