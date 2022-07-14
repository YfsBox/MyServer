#include "../include/Server.h"
#include "../include/Threadpool.h"
#include "../include/Epoll.h"
#include "../include/Timer.h"
#include "../include/HttpParser.h"

#include <string>
#include <sys/stat.h>

#define BUFFER_SIZE 2048


Server::Server() :
        ip_("127.0.0.1"), port_(8080) {
}

Server::~Server() {

}

int Server::init(const std::string ip, const int port) {
    ip_ = ip;
    port_ = port;

    std::shared_ptr <ServerSocket> tmpSocket(new ServerSocket(ip, port));
    serverSock = tmpSocket;

    serverSock->bind();
    serverSock->listen();

    return 0;
}

void Server::run(const int max_thread_size, const int max_request_size, const int max_event_size) {
    ThreadPool pool(max_thread_size, max_request_size);
    Epoll poller((*serverSock), max_event_size);

    poller.addfd(serverSock->getListenfd(), (EPOLLIN | EPOLLET | EPOLLONESHOT), NULL);

    while (true) {
        std::vector <std::shared_ptr<Httpdata>> event_datas = poller.epoll(*serverSock, -1);//轮询到来的事件，并初始化好data结构体

        /*if(event_cnt < 0){
            //std::cout<<"The Server poller.epoll error\n";
            LOGERROR_F("The Server poller.epoll error");
            continue;
        }
        for (auto &each: event_datas) { //其中这段代码的作用在于对于就绪的事件来说，将对应的request进行解析
            //并且构造reponse
            //之所以要加上一个bind(this)是因为这个函数本身就是server内部的,有一个隐含的this指针.
            pool.pushRequest(each, std::bind(&Server::doRequest, this, std::placeholders::_1));//这个函数的参数究竟是什么呢？
        }
         */

        //接下来处理,分发给线程池.
        //std::cout<<"hand to pool......\n";
        LOGOK_F("put the epoll events to pool");
    }

}

void Server::doRequest(std::shared_ptr<void> args) {
    std::shared_ptr <Httpdata> data = std::static_pointer_cast<Httpdata>(args);

    int client_fd = data->client_->getfd();

    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);

    int start_i = 0, end_i = 0, offset = 0;
    HttpParser::PARSE_STATUS parse_state = HttpParser::PARSE_REQUESTLINE;

    while (true) {
        int recv_ret = ::recv(client_fd, buffer + end_i, BUFFER_SIZE - end_i, 0);

        if (recv_ret < 0) { //这个时候有可能是读取完了，也可能是出错了
            LOGERROR_F("The fd %d has error", client_fd);
            if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {

            }
            return;
        }
        if (recv_ret == 0) {
            LOGWARNING_F("The fd %d has been closed", client_fd);
            break;
        }

        end_i += recv_ret;
        //LOGDEBUG_C("\n%s\n", buffer);
        HttpParser::REQUEST_CODE request_code = HttpParser::parseMessage(buffer, start_i, end_i, offset, parse_state,
                                                                         data->request_.get());

        //LOGDEBUG_C("Parser finish");
        if (request_code == HttpParser::REQUEST_ERROR) {
            LOGERROR_F("The request code error");
            return;
        } else if (request_code == HttpParser::REQUEST_NO) {
            LOGDEBUG_C("The request code is request no");
            continue;
        } else if (request_code == HttpParser::REQUEST_GET) {   //这里只提供了GET，如果想支持其他的，应该扩展parser，并在此增加分支
            LOGDEBUG_F("Cope with get request");
            //set version
            data->reponse_->setVersion(data->request_->version_);//获取版本号
            data->reponse_->addHeader(std::string("Server"), std::string("The Web Server of Yfs"));
            auto it = data->request_->headers_.find(Connection);//获取Connection字段
            if (it != data->request_->headers_.end()) {
                //LOGDEBUG_C("...");
                if (it->second == "keep-alive") {
                    LOGDEBUG_C("is Keep-alive");
                    data->reponse_->setAlive(true);
                    data->reponse_->addHeader("Keep-Alive", std::string("timeout=20"));//Keep-Alive
                    data->reponse_->addHeader("Connection", std::string("Keep-Alive"));//Connection
                } else {
                    LOGERROR_C("is Connection close");
                    data->reponse_->setAlive(false);
                    data->reponse_->addHeader(std::string("Connection"), std::string("close"));
                    //data->reponse_->headers_["Connection"] = "close";
                }
            }
            /*LOGDEBUG_C("Alive :%d\nKeep-alive :%s\nConnection :%s\n", data->reponse_->isalive_,
                       data->reponse_->headers_["Keep-Alive"].c_str(),
                       data->reponse_->headers_["Connection"].c_str());*/
            getMine(data);
            HttpReponse::FILE_STATUS file_status = getFile(data);

            //剩下的部分就是发送相应报文了
        }

    }

}

void Server::getMine(std::shared_ptr <Httpdata> data) {
    //LOGDEBUG_C("%s", data->request_->url_.c_str());
    //从字符串中获取文件后缀类型，进而得出mine
    std::string file_path = data->request_->url_;
    std::string mimetype;
    int pos = file_path.rfind('?');
    if (pos != std::string::npos) {
        file_path.erase(pos);
    }
    pos = file_path.rfind('.');
    if (pos != std::string::npos) {
        mimetype = file_path.substr(pos);
    }
    auto it = HttpReponse::MimeMap.find(mimetype);
    if (it != HttpReponse::MimeMap.end()) {
        data->reponse_->setMime(it->second);
    } else {
        data->reponse_->setMime("text/plain");
    }
    data->reponse_->setFilepath(file_path);
    /*LOGDEBUG_C("The file_path is %s\nMime is %s\n", data->reponse_->filepath_.c_str(),
               data->reponse_->mimeType_.type_.c_str());*/
}

HttpReponse::FILE_STATUS Server::getFile(std::shared_ptr <Httpdata> data) {
    struct stat file_stat;
    std::string file = ".." + data->reponse_->filepath_;

    LOGDEBUG_C("The file is %s",file.c_str());

    int ret_stat = stat(file.c_str(), &file_stat);
    data->reponse_->setMime("text/html");
    if (data->reponse_->filepath_ == "/") { //这个时候访问的是默认的主页
        data->reponse_->setStatusMsg("OK");
        data->reponse_->setStatus(k200Ok);
        LOGDEBUG_C("The reponse file is ok");

        return HttpReponse::FILE_OK;

    } else if (ret_stat < 0) { //找不到的情况
        data->reponse_->setStatusMsg("Not Found");
        data->reponse_->setStatus(k404NotFound);
        LOGDEBUG_C("The reponse file is Not Found");

        return HttpReponse::FILE_NOTFOUND;

    } else if (!S_ISREG(file_stat.st_mode)) { //权限不够的情况
        data->reponse_->setStatusMsg("ForBidden");
        data->reponse_->setStatus(k403forbiden);
        LOGDEBUG_C("The reponse file is forbidden");

        return HttpReponse::FILE_FORBIDDEN;
    }

    data->reponse_->setStatusMsg("OK");
    data->reponse_->setStatus(k200Ok);
    data->reponse_->setFilepath(file);

    return HttpReponse::FILE_OK;
}

//GET /tutorials/other/top-20-mysql-best-practices/ HTTP/1.1        版本号解析完了，GET方法在这里是默认的
//Host: code.tutsplus.com       没什么好弄的
//User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729) 没什么好弄的
//Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
//Accept-Language: en-us,en;q=0.5 无
//Accept-Encoding: gzip,deflate 无
//Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7
//Keep-Alive: 300
//Connection: keep-alive
//Cookie: PHPSESSID=r2t5uvjq435r4q7ib3vtdjq120
//Pragma: no-cache
//Cache-Control: no-cache


//HTTP/1.x 200 OK
//Transfer-Encoding: chunked
//Date: Sat, 28 Nov 2009 04:36:25 GMT
//Server: LiteSpeed
//Connection: close
//X-Powered-By: W3 Total Cache/0.8
//Pragma: public
//Expires: Sat, 28 Nov 2009 05:36:25 GMT
//Etag: "pub1259380237;gz"
//Cache-Control: max-age=3600, public
//Content-Type: text/html; charset=UTF-8
//Last-Modified: Sat, 28 Nov 2009 03:50:37 GMT
//X-Pingback: https://code.tutsplus.com/xmlrpc.php
//Content-Encoding: gzip
//Vary: Accept-Encoding, Cookie, User-Agent

