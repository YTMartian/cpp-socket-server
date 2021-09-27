//
// Created by tim on 2021/9/26.
//

#ifndef EPOLL_GETPROCESSOR_H
#define EPOLL_GETPROCESSOR_H

#include "head.h"
#include "Processor.h"

class GetProcessor: public Processor{
private:
    int client_sockfd;
public:
    GetProcessor();
    ~GetProcessor();
    void run() override;
    void set_client_sockfd(int new_fd) override;
};


#endif //EPOLL_GETPROCESSOR_H
