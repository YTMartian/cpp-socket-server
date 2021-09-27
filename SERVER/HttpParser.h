//
// Created by tim on 2021/9/25.
//

#ifndef EPOLL_HTTPPARSER_H
#define EPOLL_HTTPPARSER_H

#include "head.h"

class HttpParser {
private:
    string method;//get or post.
    string http_version;
public:
    HttpParser();

    void parse(char *request, int request_len);

    string get_method();
};


#endif //EPOLL_HTTPPARSER_H
