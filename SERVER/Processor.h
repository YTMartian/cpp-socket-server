//
// Created by tim on 2021/9/26.
//

#ifndef SERVER_PROCESSOR_H
#define SERVER_PROCESSOR_H

#include "head.h"
#include "Redis.h"

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
    Redis *redis;
    bool USE_REDIS;
    bool keep_alive;
public:
    //在基类析构函数声明为virtual的时候，delete基类指针，会先调用派生类的析构函数，再调用基类的析构函数。
    virtual ~Processor() = default;

    virtual void run() = 0;//开始执行

    virtual void set_client_sockfd(int new_fd) = 0;

    [[maybe_unused]] virtual llhttp_method get_method() = 0;

    virtual void set_message_body(const string &body) = 0;

    void add_head(const string &field, const string &value) {
        heads[field] = value;
    }

    string get_url() {
        return this->url;
    }

    void set_url(const string &s) {
        this->url = s;
    }

    void set_redis(Redis *redis) {
        this->redis = redis;
    }

    void set_use_redis(bool _) {
        this->USE_REDIS = _;
    }

    bool get_keep_alive() {
        return keep_alive;
    }

    int get_client_sockfd() {
        return client_sockfd;
    }
};

//纯虚析构函数需要提供函数的实现，而一般纯虚函数不能有实现，这样的原因在于，纯虚析构函数最终需要被调用，以析构基类对象.
//Processor::~Processor() = default;

#endif //SERVER_PROCESSOR_H
