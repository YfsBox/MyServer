#include "../include/Timer.h"
#include "../include/Log.h"
#include <sys/time.h>

const size_t TimerManager::DEFAULT_TIMEOUT = 20 * 1000; //20s


static size_t getnow() {
    struct timeval now_time;
    gettimeofday(&now_time, NULL);

    return ((now_time.tv_sec % 10000) * 1000) + (now_time.tv_usec / 1000);
}


TimerNode::TimerNode(std::shared_ptr <Httpdata> sdata, const size_t timeout) :
        isDeleted_(false), data_(data_) {

    expired_ = getnow() + timeout;
}

TimerNode::~TimerNode() {
    //应该关闭Httpdata中的连接
    data_->closeTimer();
}

TimerNode::TimerNode(const TimerNode &timernode) {
    isDeleted_ = timernode.getDeleted();
    expired_ = timernode.getExpired();
    data_ = timernode.data_;
}

bool TimerNode::isValid() {
    size_t now = getnow();

    if (now < expired_) {
        return true;
    } else {
        setDeleted(false);
        return false;
    }
}

void TimerNode::updateTime(const size_t timeout) {
    expired_ = getnow() + timeout;
}

void TimerManager::HandleExpired() {
    MutexGuard guard(mutex_);
    while (!timerQueue_.empty()) {
        spTimerNode node = timerQueue_.top();
        if (node->getDeleted()) {
            timerQueue_.pop();
        } else if (node->isValid() == false) {
            timerQueue_.pop();
        } else {
            break;
        }
    }
}

int TimerManager::addTimerNode(std::shared_ptr <Httpdata> sdata, const size_t timeout) {
    spTimerNode new_node(new TimerNode(sdata, timeout));

    MutexGuard guard(mutex_);
    timerQueue_.push(new_node);

    return 0;
}


