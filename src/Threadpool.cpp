//
// Created by yfs on 2022/6/26.
//
#include "../include/Threadpool.h"
#include <iostream>
#include <memory>
#include <pthread.h>


ThreadPool::ThreadPool(const int max_thread_size, const int max_request_size):
    max_thread_size_(max_thread_size),max_request_size_(max_request_size),
    running_cnt_(0),cond_(mutex_){
    if(max_thread_size_ <= 0 || max_thread_size_ > MAX_THREAD_SIZE){
        max_thread_size_ = MAX_THREAD_SIZE;
    }
    if(max_request_size_ <= 0 || max_request_size_ > MAX_REQUEST_SIZE) {
        max_request_size_ = MAX_REQUEST_SIZE;
    }
    threads_.resize(max_thread_size_);
    for(int i = 0;i < max_thread_size_;i++){
        int ret = pthread_create(&threads_[i],NULL,worker, this);
        if(ret < 0){
            //std::cout<<" The Thread create Error!\n";
            LOGERROR_F("The Thread create Error");
            throw std::exception();
        }
        running_cnt_ ++;
    }
}

ThreadPool::~ThreadPool() {
    {
        MutexGuard(this->mutex_);
        cond_.signalAll();
    }
    for(int i = 0;i < max_thread_size_;i++){
        pthread_join(threads_[i],NULL);
    }
    threads_.clear();
}

int ThreadPool::pushRequest(std::shared_ptr<void> args, std::function<void(std::shared_ptr < void > )> func) {
    MutexGuard(this->mutex_);
    if(request_.size() >= MAX_REQUEST_SIZE) {
        //std::cout<<"The Request list is full\n";
        LOGERROR_F("The Request list is full");
        return -1;
    }
    Task task;
    task.args = args;
    task.process = func;
    request_.push_back(task);
    cond_.signal();

    return 0;
}

void ThreadPool::runThread() {
    while (true){
        Task task;
        {
            MutexGuard(this->mutex_);
            while (request_.empty()) {
                cond_.wait();
            }
            task = request_.front();
            request_.pop_front();
        }
        task.process(task.args);
    }
}

void* ThreadPool::worker(void *args) {
    ThreadPool* pool = static_cast<ThreadPool*> (args);
    if(pool == NULL){
        //std::cout<<"The worker error\n";
        LOGERROR_F("The worker error");
        return NULL;
    }
    pool->runThread();
    return NULL;
}



