//
// Created by 杨丰硕 on 2022/6/26.
//
#ifndef MYSERVER_MUTEX_H
#define MYSERVER_MUTEX_H

#include <iostream>
#include <pthread.h>
#include <thread>


#define DEBUG_INFO(format, ...) printf("File:%s, Line:%d, Function:%s, %s\n", \
    __FILE__, __LINE__ , __FUNCTION__, ##__VA_ARGS__);

class Mutex{
 public:
    Mutex() {
        pthread_mutex_init(&lock_,NULL);
    }
    ~Mutex() {
        pthread_mutex_destroy(&lock_);
    }

    void lock() {
        pthread_mutex_lock(&lock_);
    }

    void unlock() {
        pthread_mutex_unlock(&lock_);
    }

    pthread_mutex_t *getMutex() { return &lock_; }

    Mutex(const Mutex&) = delete;
    Mutex& operator = (const Mutex&) = delete;

 private:
    pthread_mutex_t lock_;
};

class MutexGuard {
public:
    explicit MutexGuard(Mutex &lock):lock_(lock) {
       /* std::cout<<"The lock thread id is "<<std::this_thread::get_id()<<'\n';
        DEBUG_INFO("%s","LOCK");*/
        lock_.lock();
    }
    ~MutexGuard() {
        /*std::cout<<"The unlock thread id is "<<std::this_thread::get_id()<<'\n';
        DEBUG_INFO("%s","UNLOCK");*/
        lock_.unlock();
    }

    MutexGuard(const MutexGuard&) = delete;
    MutexGuard &operator = (const MutexGuard&) = delete;
private:
    Mutex &lock_;
};

#endif //MYSERVER_MUTEX_H
