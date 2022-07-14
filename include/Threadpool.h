//
// Created by 杨丰硕 on 2022/6/26.
//

#ifndef MYSERVER_THREADPOOL_H
#define MYSERVER_THREADPOOL_H

#include "Condition.h"
#include "Log.h"
#include <list>
#include <memory>
#include <vector>
#include <functional>

struct Task {
    std::shared_ptr<void> args;
    std::function<void(std::shared_ptr<void>)> process;
};

class ThreadPool{
public:
    static const int MAX_THREAD_SIZE = 1024;
    static const int MAX_REQUEST_SIZE = 10000;

    ThreadPool(const int max_thread_size,const int max_request_size);
    ~ThreadPool();

    int pushRequest(std::shared_ptr<void> args,std::function<void(std::shared_ptr<void>)> func);

private:
    void runThread();
    static void *worker(void *args);

private:
    std::vector<pthread_t> threads_;
    std::list<Task> request_;
    int running_cnt_;
    int max_thread_size_;
    int max_request_size_;

    Mutex mutex_;
    Condition cond_;
};
#endif //MYSERVER_THREADPOOL_H
