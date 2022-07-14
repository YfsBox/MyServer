#include <iostream>
#include "include/Server.h"
#include "include/Threadpool.h"
#include "include/Epoll.h"
#include "include/Log.h"
#include "include/HttpRequest.h"
#include "include/HttpReponse.h"
#include "include/HttpParser.h"
#include "include/Timer.h"


int main(int argc,char **argv) {
    std::cout << "Hello, World!" << '\n';
    //Server *server = Server::getInstance();
    Log* logptr = Log::getInstance();
    logptr->init(LOG_LEVEL_NONE,LOG_TARGET_FILE,"fortest.log","../");
    //logptr->init(LOG_LEVEL_NONE,LOG_TARGET_FILE,"","");
    Server server;
    server.init("0.0.0.0",10099,Epoll::DEFALUT_EVENT); //这个地方使用127.0.0.1的活,只有本地才会听到,这样是不对的
    server.run(ThreadPool::MAX_THREAD_SIZE,ThreadPool::MAX_REQUEST_SIZE,Epoll::MAX_EVENT_SIZE);
    return 0;
}
