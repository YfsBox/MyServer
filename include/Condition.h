//
// Created by 杨丰硕 on 2022/6/26.
//

#ifndef MYSERVER_CONDITION_H
#define MYSERVER_CONDITION_H

#include "Mutex.h"
#include <pthread.h>

class Condition {
public:
    explicit Condition(Mutex &mutex):mutex_(mutex) {
        pthread_cond_init(&cond_,NULL);
    }
    ~Condition() {
        pthread_cond_destroy(&cond_);
    }
    void wait() {
        pthread_cond_wait(&cond_,mutex_.getMutex());
    }
    void signal() {
        pthread_cond_signal(&cond_);
    }
    void signalAll() {
        pthread_cond_broadcast(&cond_);
    }

private:
    Mutex &mutex_;
    pthread_cond_t cond_;
};

#endif //MYSERVER_CONDITION_H
