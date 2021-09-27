//
// Created by tim on 2021/9/26.
//

#ifndef EPOLL_POSTPROCESSOR_H
#define EPOLL_POSTPROCESSOR_H

#include "head.h"
#include "Processor.h"

class PostProcessor : public Processor {
private:
    int client_sockfd;
    Json::Reader json_reader;
    Json::Value message_body;
public:
    PostProcessor();
    ~PostProcessor();
    void run() override;
    void set_client_sockfd(int new_fd) override;
};


#endif //EPOLL_POSTPROCESSOR_H
