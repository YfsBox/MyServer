#ifndef MYSERVER_HTTPREQUEST_H
#define MYSERVER_HTTPREQUEST_H

#include "Httpbase.h"
#include <string>
#include <iostream>
#include <unordered_map>


class HttpRequest {
public:

    friend std::ostream &operator<<(std::ostream & os,const HttpRequest &request);

    HttpRequest(HTTP_METHOD method = METHOD_NOT_SUPPORT,
                HTTP_VERSION version = VERSION_NOT_SUPPORT,
                const std::string url = std::string(""),
                const std::string content = std::string("")):
                    method_(method),version_(version),url_(url),
                    headers_(std::unordered_map<HTTP_HEADER,std::string,EnumClassHash>()),body_("") {};
    
    struct EnumClassHash {
        template<typename T>
        std::size_t operator() (T t) const {
            return static_cast<std::size_t> (t);//hash值是将此转型为整型.
        }
    };

    static std::unordered_map<std::string,HTTP_HEADER> header_map;
    //由字符串得出枚举

//private:
    HTTP_METHOD method_;
    HTTP_VERSION version_;
    std::string url_;
    //根据枚举的kv map
    std::unordered_map<HTTP_HEADER,std::string,EnumClassHash> headers_;
    std::string body_;

};

std::ostream &operator<<(std::ostream & os,const HttpRequest &request);

#endif