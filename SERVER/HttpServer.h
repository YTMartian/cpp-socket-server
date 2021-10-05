//
// Created by tim on 2021/9/25.
//

#ifndef SERVER_HTTPSERVER_H
#define SERVER_HTTPSERVER_H

#include "head.h"
#include "Processor.h"
#include "GetProcessor.h"
#include "PostProcessor.h"
#include "ProcessorFactory.h"
#include "UrlEncoder.h"
#include "Redis.h"

class HttpServer {
private:
    struct sockaddr_in *server_address{};
    struct sockaddr_in *client_address{};
    int port;
    int backlog;
    int server_sockfd{};
    char *request_buf;
    int epoll_fd{};
    struct epoll_event event{};
    struct epoll_event events[MAX_EVENTS]{};//events:即wait event
    Processor *processor;
    ProcessorFactory processor_factory;
    llhttp_t http_parser{};
    llhttp_settings_t *http_parser_settings;
    UrlEncoder url_encoder;
    vector<string> head_fields, head_values;//临时存放heads
    Redis *redis;
    bool USE_REDIS;
public:

    explicit HttpServer(bool use_redis=false);

    ~HttpServer();

    //set AF_INET socket, bind it and listen it.
    int get_ipv4_socket_bind_and_listen();

    int set_port(int new_port);

    int set_backlog(int new_backlog);

    void set_callbacks(llhttp_settings_t *settings);

    void run();
/*
 * 通过静态函数调用非静态函数
 * https://blog.csdn.net/zzhongcy/article/details/41981855
 */
#define HS (reinterpret_cast<HttpServer*>(parser->data))
public:
    static int callback_on_message_begin(llhttp_t *parser) {
        return HS->on_message_begin();
    }

    static int callback_on_url(llhttp_t *parser, const char *at, size_t length) {
        return HS->on_url(parser, at, length);
    }

    static int callback_on_header_field(llhttp_t *parser, const char *at, size_t length) {
        return HS->on_header_field(at, length);
    }

    static int callback_on_header_value(llhttp_t *parser, const char *at, size_t length) {
        return HS->on_header_value(at, length);
    }

    static int callback_on_headers_complete(llhttp_t *parser) {
        return HS->on_headers_complete();
    }

    static int callback_on_body(llhttp_t *parser, const char *at, size_t length) {
        return HS->on_body(at, length);
    }

    static int callback_on_message_complete(llhttp_t *parser) {
        return HS->on_message_complete();
    }

#undef HS

private:

    int on_message_begin();

    int on_url(llhttp_t *parser, const char *at, size_t length);

    int on_header_field(const char *at, size_t length);

    int on_header_value(const char *at, size_t length);

    int on_headers_complete();

    int on_body(const char *at, size_t len);

    int on_message_complete();
};


#endif //SERVER_HTTPSERVER_H
