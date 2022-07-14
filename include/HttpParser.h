#ifndef MYSERVER_HTTPPARSER_H
#define MYSERVER_HTTPPARSER_H

#include "Httpdata.h"
#include "HttpRequest.h"
#include <string>
#include <string.h>

#define NO_ACTION

const char RC = '\r';
const char NC = '\n';
const char STR_END = '\0';


class HttpParser {
public:
    enum PARSE_STATUS {
        PARSE_REQUESTLINE = 0,
        PARSE_HEADERS,
        PARSE_BODY
    };
    enum LINE_STATUS {
        LINE_OK = 0,
        LINE_MORE,
        LINE_ERROR
    };
    enum REQUEST_CODE {
        REQUEST_GET = 0,
        REQUEST_ERROR,
        REQUEST_NO
    };

    static LINE_STATUS parseLine(char *buffer, int &start_i, int &end_i);

    static REQUEST_CODE parseRequestLine(char *line, PARSE_STATUS &parse_status,  HttpRequest *request);

    static REQUEST_CODE parseHeaders(const char *line, PARSE_STATUS &parse_status, HttpRequest *request);

    static REQUEST_CODE parseBody(const char *buffer, HttpRequest *request);

    static REQUEST_CODE
    parseMessage(char *buffer, int &start_i, int &end_i, int begin_offset, PARSE_STATUS &parse_status,
                 HttpRequest *request);
};

# endif