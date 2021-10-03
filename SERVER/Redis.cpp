//
// Created by tim on 2021/10/1.
//

#include "Redis.h"

Redis::Redis() {
    this->host = "127.0.0.1";
    this->port = 6379;
    this->_connect = nullptr;
    this->_reply = nullptr;
}

Redis::~Redis() {
    redisFree(_connect);//释放redisConnect()所产生的连接
    delete _connect;
    delete _reply;
}

void Redis::connect() {
    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    _connect = redisConnectWithTimeout(host.c_str(), port, timeout);
    if (_connect != nullptr && _connect->err) {
        spdlog::error("redis连接失败: {} {}({})", _connect->errstr, __FILE__, __LINE__);
        logger->error("redis连接失败: {} {}({})", _connect->errstr, __FILE__, __LINE__);
        throw runtime_error("redis连接失败");
    }
}

string Redis::get(const string& key) {
    _reply = (redisReply *) redisCommand(_connect, "GET %s", key.c_str());
    if(_reply == nullptr) {
        logger->error("GET {}执行失败:{}({})", key.c_str(), __FILE__, __LINE__);
        throw runtime_error("");
    }
    if(_reply->type == REDIS_REPLY_NIL) {
        throw REDIS_REPLY_NIL;
    }
    string str = _reply->str;
    freeReplyObject(_reply);
    return str;
}

void Redis::set(string key, string value) {
    _reply = (redisReply *) redisCommand(_connect, "SET %s %s", key.c_str(), value.c_str());
    freeReplyObject(_reply);//释放redisCommand执行后返回的redisReply所占用的内存
}

void Redis::set_host(const string &host) {
    this->host = host;
}

void Redis::set_port(int port) {
    this->port = port;
}