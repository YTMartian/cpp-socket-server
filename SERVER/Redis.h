//
// Created by tim on 2021/10/1.
//

#ifndef SERVER_REDIS_H
#define SERVER_REDIS_H

#include "head.h"

class Redis {

private:
    redisContext *_connect;
    redisReply *_reply;
    string host;
    int port;
public:

    Redis();

    ~Redis();

    void connect();

    string get(const string& key);

    void set(string key, string value);

    void set_host(const string &host);

    void set_port(int port);

};


#endif //SERVER_REDIS_H
