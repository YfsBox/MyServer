#ifndef MYSERVER_TIMER_H
#define MYSERVER_TIMER_H

#include "Mutex.h"
#include "Httpdata.h"
#include <memory>
#include <string>
#include <queue>
#include <deque>
#include <unistd.h>

class Httpdata;

class TimerNode {
public:
    TimerNode(std::shared_ptr<Httpdata> sdata,const size_t timeout);
    ~TimerNode();
    TimerNode(const TimerNode& timenode);

    void updateTime(const size_t timeout);
    bool isValid();


    void setDeleted(const bool deleted) { isDeleted_ = deleted; }
    bool getDeleted() const { return isDeleted_; }
    size_t getExpired() const { return expired_; }


private:
    bool isDeleted_;
    size_t expired_;
    std::shared_ptr<Httpdata> data_;
};

struct TimerCmp {
    bool operator() (std::shared_ptr<TimerNode> &a,
                     std::shared_ptr<TimerNode> &b) const {
        return a->getExpired() > b->getExpired();
    }
};
//是对于一个优先队列的抽象
class TimerManager {
public:

    void HandleExpired();
    int addTimerNode(std::shared_ptr<Httpdata> sdata,const size_t timeout);//将一个client对应的节点追加到优先级队列上
    const static size_t DEFAULT_TIMEOUT;
    int getSize() const { return timerQueue_.size(); }
private:
    typedef std::shared_ptr<TimerNode> spTimerNode;
    std::priority_queue< spTimerNode,std::deque<spTimerNode>,TimerCmp > timerQueue_;
    
    Mutex mutex_;

};

# endif