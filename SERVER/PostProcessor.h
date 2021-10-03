//
// Created by tim on 2021/9/26.
//

#ifndef SERVER_POSTPROCESSOR_H
#define SERVER_POSTPROCESSOR_H

#include "head.h"
#include "Processor.h"

class PostProcessor : public Processor {
private:
public:
    PostProcessor();

    ~PostProcessor();

    void run() override;

    void set_client_sockfd(int new_fd) override;

    llhttp_method get_method() override;

    void set_message_body(const string &body) override;

};


#endif //SERVER_POSTPROCESSOR_H
