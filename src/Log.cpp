#include "../include/Log.h"
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <cstdarg>
#include <iostream>


Log* Log::getInstance() {
    static Log logInstance;
    return &logInstance;
}

void* Log::logThreadFunc(void *args) {
    Log *logger = static_cast<Log *>(args);
    if(logger == NULL){
        std::cout<<"the logger is null in log thread\n";
        return NULL;
    }
    while(1){
        MutexGuard guard(logger->logMutex_);
        if(logger->queue_.empty()){
            //std::cout<<"The log thread is waiting......\n";
            logger->cond_->wait();
        }
        //std::cout<<"The log thread is waking......\n";
        //std::cout<<"The front is "<<logger->queue_.front()<<'\n';
        std::string request;
        std::cout << "the size is %d" << logger->queue_.size() << '\n';
        request = logger->queue_.front();

        fwrite(request.c_str(), request.length(), 1,logger->logfile_ptr_);
        fflush(logger->logfile_ptr_);
        logger->queue_.pop_front();
    }
    return NULL;
}

void Log::init(LOGLEVEL level,LOGTARGET target,const std::string fname,const std::string fpath) {
    //setLevel(level);
    //setTarget(target);

    cond_.reset(new Condition(logMutex_));

    std::string name = fpath + fname;
    if(name.empty()){
    }else {
        file_name_ = fname;
    }
    /*std::cout<<"the file name is:\n"<<file_name_<<'\n';
    std::cout<<name<<'\n';*/
    FILE* fptr = ::fopen(name.c_str(),"a+b");
    if(fptr == NULL){
        std::cout<<"The log file open error!\n";
        exit(0);
    }
    logfile_ptr_ = fptr;

    int thread_ret = pthread_create(&logthread_,NULL,logThreadFunc,this);
    if(thread_ret < 0){
        std::cout<<"create log thread error\n";
        exit(0);
    }
}
//这一部分操作主要是涉及到根据传入的参数,构造一个字符串.
void Log::addRequest(
                        LOGLEVEL loglevel,
                        LOGTARGET logtarget,
                        const char* fileName,   // 函数所在文件名
                        const char* function,   // 函数名
                        int lineNumber,            // 行号
                        const char* format,              // 格式化
	                    ...){
    
    char msg[256] = { 0 };
    va_list vArgList;                            
    va_start(vArgList,format);
    vsnprintf(msg, 256,format,vArgList);
    va_end(vArgList);

    time_t now = time(NULL);
    struct tm* tmstr = localtime(&now);

    const char* logLevel;
	if (loglevel == LOG_LEVEL_DEBUG){
		logLevel = "DEBUG";
	}
	else if (loglevel == LOG_LEVEL_WARNING){
		logLevel = "WARNING";
	}
	else if (loglevel == LOG_LEVEL_ERROR){
		logLevel = "ERROR";
	}else if(loglevel == LOG_LEVEL_OK) {
        logLevel = "OK";
    }

    char content[512] = { 0 };
    sprintf(content, "[%04d-%02d-%02d %02d:%02d:%02d][%s][%s:%d %s]%s\n",
                tmstr->tm_year + 1900,
                tmstr->tm_mon + 1,
                tmstr->tm_mday,
                tmstr->tm_hour,
                tmstr->tm_min,
                tmstr->tm_sec,
                logLevel,
                fileName,
                lineNumber,
                function,
                msg);
    if(logtarget == LOG_TARGET_FILE){
        {
            MutexGuard guard(logMutex_);
            queue_.push_back(content);
        }
        cond_->signal();
    }else if(logtarget == LOG_TARGET_CONSULE){
        std::cout<<content;//这样的话是否存在线程安全问题呢???
    }
}
