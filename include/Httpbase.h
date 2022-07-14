#ifndef MYSERVER_HTTPBASE_H
#define MYSERVER_HTTPBASE_H

enum HTTP_VERSION {
    HTTP_10 = 0,
    HTTP_11,
    VERSION_NOT_SUPPORT
};

enum HTTP_STATUS {
    Unknow,
    k200Ok = 200,
    k403forbiden = 403,
    k404NotFound = 404
};

enum HTTP_METHOD {
    GET = 0,
    POST,
    PUT,
    DELETE,
    METHOD_NOT_SUPPORT
};

enum HTTP_HEADER {
    Host = 0,
    User_Agent,
    Connection,
    Accept_Encoding,
    Accept_Language,
    Accept,
    Cache_Control,
    Upgrade_Insecure_Requests
};


#endif