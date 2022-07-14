#ifndef MYSERVER_LOG_H
#define MYSERVER_LOG_H

#include <syslog.h>
#include <string>
#include <list>
#include <pthread.h>
#include <memory>
#include "Mutex.h"
#include "Condition.h"



enum LOGLEVEL {
    LOG_LEVEL_NONE,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_OK
};

enum LOGTARGET {
    LOG_TARGET_NONE = 0x00,
    LOG_TARGET_CONSULE = 0x01,    
    LOG_TARGET_FILE = 0x10,
};


class Log {
public:
    static Log* getInstance();
    void addRequest(
                            LOGLEVEL loglevel,         // Log级别
                            LOGTARGET logtarget,
                            const char* fileName,   // 函数所在文件名
                            const char* function,   // 函数名
                            int lineNumber,            // 行号
                            const char* format,              // 格式化
	                    ...);
    //下一步进行扩展,增加线程号来进行标识.
    void init(LOGLEVEL,LOGTARGET,const std::string fname,const std::string fpath);


private:
    static void *logThreadFunc(void *args);

private:
    Log() {};
    ~Log() {};

    Mutex logMutex_;
    std::shared_ptr<Condition> cond_;

    std::string logBuf_;
//level,target

    // log file
    FILE *logfile_ptr_;
    std::string file_name_;
    //std::string file_path_;

    // event list
    std::list<std::string> queue_;
    pthread_t logthread_;

};

#define LOGERROR_F(...)       Log::getInstance()->addRequest(LOG_LEVEL_ERROR,LOG_TARGET_FILE,__FILE__,__PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#define LOGDEBUG_F(...)       Log::getInstance()->addRequest(LOG_LEVEL_DEBUG,LOG_TARGET_FILE,__FILE__,__PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#define LOGWARNING_F(...)     Log::getInstance()->addRequest(LOG_LEVEL_WARNING,LOG_TARGET_FILE,__FILE__,__PRETTY_FUNCTION__,__LINE__,__VA_ARGS__)
#define LOGOK_F(...)          Log::getInstance()->addRequest(LOG_LEVEL_OK,LOG_TARGET_FILE,__FILE__,__PRETTY_FUNCTION__,__LINE__,__VA_ARGS__)

#define LOGERROR_C(...)       Log::getInstance()->addRequest(LOG_LEVEL_ERROR,LOG_TARGET_CONSULE,__FILE__,__PRETTY_FUNCTION__,__LINE__, __VA_ARGS__)
#define LOGDEBUG_C(...)       Log::getInstance()->addRequest(LOG_LEVEL_DEBUG,LOG_TARGET_CONSULE,__FILE__,__PRETTY_FUNCTION__,__LINE__, __VA_ARGS__)
#define LOGWARNING_C(...)     Log::getInstance()->addRequest(LOG_LEVEL_WARNING,LOG_TARGET_CONSULE,__FILE__,__PRETTY_FUNCTION__,__LINE__,__VA_ARGS__)
#define LOGOK_C(...)          Log::getInstance()->addRequest(LOG_LEVEL_OK,LOG_TARGET_CONSULE,__FILE__,__PRETTY_FUNCTION__,__LINE__,__VA_ARGS__)

# endif