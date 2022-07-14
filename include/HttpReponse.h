#ifndef MYSERVER_HTTPREPONSE_H
#define MYSERVER_HTTPREPONSE_H

#include "Httpbase.h"
#include "Log.h"
#include <iostream>
#include <string>
#include <unordered_map>

//关于MimeType这一部分的内容,后来再查查文档,补充这一部分的知识.
struct MimeType {
    MimeType(const char *str) : type_(str) {};

    MimeType(const std::string &str) : type_(str) {};

    std::string type_;
};

class HttpReponse {

public:
    enum FILE_STATUS {
            FILE_OK = 0,
            FILE_NOTFOUND,
            FILE_FORBIDDEN
    };

    HttpReponse(HTTP_VERSION verison = VERSION_NOT_SUPPORT,
                HTTP_STATUS status = Unknow,
                const std::string body = std::string("")) :
            isalive_(true), version_(verison), status_code_(status),
            mimeType_("test/html"), headers_(std::unordered_map<std::string, std::string>()),
            body_(body) {};

    ~HttpReponse() {}

    void addHeader(const std::string &k, const std::string &value) {
        headers_[k] = value;
    }

    void writeMessage(char *buffer) const;

    void setVersion(HTTP_VERSION version) { version_ = version; }

    void setAlive(bool alive) { isalive_ = alive; }

    void setFilepath(const std::string &path) { filepath_ = path; }

    void setMime(const char *mime) { mimeType_.type_ = mime; }

    void setMime(const MimeType &mime) { mimeType_ = mime; }

    void setStatusMsg(const char *msg) {status_msg_ = msg;}

    void setStatus(const HTTP_STATUS httpstatus) { status_code_ = httpstatus; }


    static std::unordered_map <std::string, MimeType> MimeMap;

    bool isalive_;//根据request中根据connect来确定的
    HTTP_VERSION version_; //版本号是根据request中的版本号确定的
    HTTP_STATUS status_code_; //取决于查找请求资源的结果
    MimeType mimeType_; //通过解析资源类型确定
    std::string status_msg_;
    std::string body_;
    std::string filepath_; //也是根据请求资源的解析结果来确定的
    std::unordered_map <std::string, std::string> headers_;
};

#endif