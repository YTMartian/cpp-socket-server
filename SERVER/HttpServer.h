//
// Created by tim on 2021/9/25.
//

#ifndef EPOLL_HTTPSERVER_H
#define EPOLL_HTTPSERVER_H

#include "head.h"
#include "Processor.h"
#include "GetProcessor.h"
#include "PostProcessor.h"

class HttpServer {
private:
    struct sockaddr_in *server_address{};
    struct sockaddr_in *client_address{};
    int port;
    int backlog;
    int server_sockfd;
    char *request_buf;
    int epoll_fd;
    struct epoll_event event{};
    struct epoll_event events[MAX_EVENTS];//events:即wait event
    Processor *processor;
public:

    HttpServer();

    ~HttpServer();

    //set AF_INET socket, bind it and listen it.
    int get_ipv4_socket_bind_and_listen();

    //analyze the request string and get the request file name.
    string get_request_file_name(string buf);

    void handle_get(int client_sockfd, string file_name);

    void handle_post(int client_sockfd, string buf);

    string get_time();

    int get_port();

    int set_port(int new_port);

    int set_backlog(int new_backlog);

    int get_backlog();

    void run();
};


#endif //EPOLL_HTTPSERVER_H
