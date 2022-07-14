#include "../include/HttpReponse.h"
//这个表提供给reponse类,用来根据请求文件的后缀得出mime类型的
std::unordered_map<std::string,MimeType> HttpReponse::MimeMap = {
        {"","text/plain"},
        {".txt","text/plain"},
        {".html","text/html"},
        {".css","text/css"},
        {".rtf","application/rtf"},
        {".gif","image/gif"},
        {".jpg","image/jpeg"},
        {".jpeg","image/jpeg"},
        {".au", "audio/basic"},
        {".mpeg", "video/mpeg"},
        {".mpg", "video/mpeg"},
        {".avi", "video/x-msvideo"},
        {".gz", "application/x-gzip"},
        {".tar", "application/x-tar"},
};

//根据reponse的字段值，写出报文到缓冲区中
void HttpReponse::writeMessage(char *buffer) const {
    if(version_ == HTTP_11){
        sprintf(buffer,"HTTP/1.1 %d %s\r\n",status_code_,status_msg_.c_str());
    }else if(version_ == HTTP_10) {
        sprintf(buffer,"HTTP/1.0 %d %s\r\n",status_code_,status_msg_.c_str());
    }else{
        LOGERROR_F("The version not support");
    }

    for(auto it = headers_.begin() ;it != headers_.end();it ++){
        sprintf(buffer,"%s%s: %s\r\n",buffer,it->first.c_str(),it->second.c_str());
    }
    sprintf(buffer,"%sContent-type: %s\r\n",buffer,mimeType_.type_.c_str());

    if(isalive_) {
        sprintf(buffer,"%sConnection: Keep-alive\r\n",buffer);
    } else {
        sprintf(buffer,"%sConnection: close\r\n",buffer);
    }
}