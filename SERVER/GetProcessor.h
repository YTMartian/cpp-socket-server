//
// Created by tim on 2021/9/26.
//

#ifndef SERVER_GETPROCESSOR_H
#define SERVER_GETPROCESSOR_H

#include "Processor.h"

class GetProcessor : public Processor {
private:
    void send_file(const string & file_name);
public:
    GetProcessor();

    ~GetProcessor();

    void run() override;

    void set_client_sockfd(int new_fd) override;

    llhttp_method get_method() override;

    void set_message_body(const string &body) override;//get方法的body就是url后的一堆参数?a=1&b=2之类的
};


#endif //SERVER_GETPROCESSOR_H
