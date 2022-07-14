#include "../include/HttpParser.h"

#include <iostream>
#include <algorithm>
#include <string>

//用来过滤字符串中多余的部分
static std::string& trim(std::string &str){
    if(str.empty()){
        return str;
    }
    str.erase(0,str.find_first_not_of(" \t"));

    if(str.empty()) {
        return str;
    }

    str.erase(str.find_last_not_of(" \t") + 1);

    return str;
}

HttpParser::LINE_STATUS
HttpParser::parseLine(char *buffer,int &start_i,int &end_i) {
    char tmpc;
    for(;start_i < end_i;start_i ++){//主要在于rc的检查
        tmpc = buffer[start_i];
        if(tmpc == RC){
            if(start_i + 1 == end_i){//两种不同的情况在于末尾有没有/n
                return LINE_MORE;
            }
            if(buffer[start_i + 1] == NC) {
                buffer[start_i ++] = STR_END;//chanege r to 0
                buffer[start_i ++] = STR_END;//chenge n to 0
                return LINE_OK;
            }
            return LINE_ERROR;
        }
    }
    return LINE_MORE;
}
//这个函数主要用到各种分割字符串和字符串比较的技巧
//其正确性是等待测试的.
HttpParser::REQUEST_CODE 
HttpParser::parseRequestLine(char *line,PARSE_STATUS &parse_status,HttpRequest *request) {
    char *method;
    char *url;
    char *version;
    
    url = strpbrk(line," \t");
    if(url == NULL) {
        return REQUEST_ERROR;
    }
    *url = STR_END;
    url ++;
    url += strspn(url," \t");//过滤\t

    method = line;
    //LOGDEBUG_C("Method :%s",method);
    version = strpbrk(url," \t");
    if(version == NULL) {
        return REQUEST_ERROR;
    }
    *version = STR_END;
    version ++;
    version += strspn(version," \t");
    /*LOGDEBUG_C("Url :%s",url);
    LOGDEBUG_C("Version :%s",version);*/

    if(strcasecmp(method,"GET") == 0){
        request->method_ = GET;
    }else if(strcasecmp(method,"POST") == 0){
        request->method_ = POST;
    }else if(strcasecmp(method,"PUT") == 0){
        request->method_ = PUT;
    }else {
        request->method_ = METHOD_NOT_SUPPORT;
        return REQUEST_ERROR;
    }

    if(strncasecmp(url,"http://",7) == 0){
        url += 7;
        url = strchr(url,'/');

    }else if(strncasecmp(url,"/",1) == 0) {
        NO_ACTION;
    }else {
        return REQUEST_ERROR;
    }
    if(url == NULL || *url != '/'){
        return REQUEST_ERROR;
    }

    request->url_ = url;
    //LOGDEBUG_C("%s",request->url_);
    //std::cout<<"The request url is"<< request->url_ << " and url :" <<url<<'\n';
//version
    if(strncasecmp("HTTP/1.1",version,8) == 0){
        request->version_ = HTTP_11;
    }else if(strncasecmp("HTTP/1.0",version,8) == 0){
        request->version_ = HTTP_10;
    }else {
        return REQUEST_ERROR;
    }
    parse_status = PARSE_HEADERS;
    //LOGDEBUG_C("Version :%s\n Method :%s\n Url :%s\n",version,method,url);
    return REQUEST_NO;
}

HttpParser::REQUEST_CODE
HttpParser::parseHeaders(const char *line,PARSE_STATUS &parse_status,HttpRequest *request) {
    //LOGDEBUG_C("Begin parse headers,the line is %s",line);
    if(*line == STR_END){
        if(request->method_ == GET){
            return REQUEST_GET;//因为GET类型是没有body的
        }
        parse_status = PARSE_BODY;
        return REQUEST_NO;//NO指的是,这个REquest这阶段的解析已经结束,还需要继续向下进行,目前没有什么异常
    }


    char key[200],value[200];
    sscanf(line,"%[^:]:%[^:]",key,value);
    //LOGDEBUG_C("key :%s \t value :%s \n",key,value);
    std::string str_k(key);
    std::transform(str_k.begin(),str_k.end(),str_k.begin(),::toupper);
    std::string str_v(value);

    //这个trim用于过滤,左侧和右侧的     \t符号
    
    auto it = HttpRequest::header_map.find(trim(str_k));
    if(it != HttpRequest::header_map.end()){ //这种key是存在的,也就是合法的
        request->headers_.insert(std::make_pair(it->second,trim(str_v)));
    }else{
        std::cout<<"not have this key in map\n";
    }
    //trim是什么东西???
    //second和first分别是什么??
    return REQUEST_NO;
}

HttpParser::REQUEST_CODE 
HttpParser::parseBody(const char *buffer,HttpRequest *request) {
    request->body_ = buffer;
    return REQUEST_GET;
}

//正常情况下,将会在body解析完或者headers解析完,返回OK
//这是一个状态机,逐行读取,根据每一行的状态，进行特定的分析,比如说header,requestline，body等
HttpParser::REQUEST_CODE
HttpParser::parseMessage(char *buffer,int &start_i,
                            int &end_i,int begin_offset,
                            PARSE_STATUS &parse_status,HttpRequest *request) {
    int linecode;
    while((linecode = parseLine(buffer,start_i,end_i)) == LINE_OK){
        char *tmp_buf = buffer + begin_offset;
        //LOGDEBUG_C("the line is %s",tmp_buf);
        begin_offset = start_i;
        switch(parse_status){
            case PARSE_REQUESTLINE:{
                int ret = parseRequestLine(tmp_buf,parse_status,request);
                if(ret == REQUEST_ERROR) {
                    return REQUEST_ERROR;
                }
                break;
            }
            case PARSE_HEADERS:{
                int ret = parseHeaders(tmp_buf,parse_status,request);
                if(ret == REQUEST_ERROR) {
                    return REQUEST_ERROR;
                } else if(ret == REQUEST_GET) {
                    return REQUEST_GET;//已经读取到一个完整无误的报文了
                }
                break;
            }   
            case PARSE_BODY:{
                int ret = parseBody(tmp_buf,request);
                if(ret == REQUEST_ERROR) {
                    return REQUEST_ERROR;
                }
                return REQUEST_GET;//
                break;
            }
            default:
                return REQUEST_ERROR;
        }
    }
    if(linecode == LINE_MORE) {//此时说明接受的缓冲区还不足以构造出一个完成的报文
        return REQUEST_NO;
    }else{
        return REQUEST_ERROR;
    }
}





