//
// Created by tim on 2021/9/26.
//

#ifndef EPOLL_PROCESSOR_H
#define EPOLL_PROCESSOR_H

#include "head.h"

//处理请求的基类
class Processor {
private:
    int client_sockfd;
public:
    virtual void run() = 0;//开始执行
    virtual void set_client_sockfd(int new_fd) = 0;
};


#endif //EPOLL_PROCESSOR_H
