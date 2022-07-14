#include "../include/HttpRequest.h"


std::unordered_map<std::string,HTTP_HEADER> HttpRequest::header_map = {
    {"HOST",                      Host},
    {"USER-AGENT",                User_Agent},
    {"CONNECTION",                Connection},
    {"ACCEPT-ENCODING",           Accept_Encoding},
    {"ACCEPT-LANGUAGE",           Accept_Language},
    {"ACCEPT",                    Accept},
    {"CACHE-CONTROL",             Cache_Control},
    {"UPGRADE-INSECURE-REQUESTS", Upgrade_Insecure_Requests}
};

std::ostream &operator<<(std::ostream &os,const HttpRequest &request){
    os << "method:" << request.method_ << "\n";
    os << "url:" << request.url_ << "\n";
    os << "version:" <<request.version_ << "\n";

    for (auto it = request.headers_.begin(); it != request.headers_.end();it ++){
        os << it->first << ":" <<it->second << "\n";
    }

    return os;
}