//
// Created by tim on 2021/9/26.
//

#ifndef SERVER_PROCESSOR_H
#define SERVER_PROCESSOR_H

#include "head.h"

using namespace std;

//处理请求的基类
class Processor {
protected:
    int client_sockfd;
    llhttp_method method;
    string url;
    map<string, string> heads;//请求头
    Json::Reader json_reader;
    Json::Value message_body;
public:
    virtual void run() = 0;//开始执行

    virtual void set_client_sockfd(int new_fd) = 0;

    [[maybe_unused]] virtual llhttp_method get_method() = 0;

    virtual int set_message_body(const string &body) = 0;

    void add_head(const string &field, const string &value) {
        heads[field] = value;
    }

    string get_url() {
        return this->url;
    }

    void set_url(const string &s) {
        this->url = s;
    }

};


#endif //SERVER_PROCESSOR_H
