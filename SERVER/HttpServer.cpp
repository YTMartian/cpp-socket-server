//
// Created by tim on 2021/9/25.
//

#include "HttpServer.h"

HttpServer::HttpServer() {
    port = DEFAULT_PORT;
    backlog = DEFAULT_BACKLOG;
    request_buf = new char[N];
    server_address = new struct sockaddr_in;
    client_address = new struct  sockaddr_in;
}

HttpServer::~HttpServer() {
    delete[] request_buf;
    delete httpParser;
    delete server_address;
    delete client_address;
}

int HttpServer::get_ipv4_socket_bind_and_listen() {
    int server_fd;
    server_address->sin_family = AF_INET;//set socket domain.
    server_address->sin_port = htons(port);//host bytes-order to network bytes-order.
    server_address->sin_addr.s_addr = INADDR_ANY;// ip address is 0.0.0.0

    //establish a socket.
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket建立失败.");
        exit(EXIT_FAILURE);//exit(1)
    }
//    cout << "server socket: " << server_fd << endl;
    //bind server socket to server address.
    if ((bind(server_fd, (struct sockaddr *) server_address, sizeof(struct sockaddr))) < 0) {
        perror("socket建立失败.");
        exit(EXIT_FAILURE);
    }
    cout << "socket建立成功." << endl;
    //listen to connection from clients.
    if (listen(server_fd, backlog) < 0) {
        perror("启动监听失败.");
        exit(EXIT_FAILURE);
    }
    cout << "监听端口: " << port << endl;
    return server_fd;
}

string HttpServer::get_request_file_name(string buf) {
    //the request buf is like "GET /index.html HTTP/1.1 ... ..."
    size_t buf_index = 0;
    string info;
    while (buf_index < buf.size() && buf[buf_index++] != '/') info += buf[buf_index - 1];
    if (buf_index - 1 == buf.size() || buf[buf_index - 1] != '/')return "";//illegal request.
    string file_name;
    //extract request file from the request buf.
    while (buf[buf_index++] != ' ') file_name += buf[buf_index - 1];
    if (!IS_TEST) cout << info + file_name << endl;
    return file_name;
}

void HttpServer::handle_get(int client_sockfd, string file_name) {
    struct stat st;
    int f;
    //if file not find then return 404.
    if (stat(file_name.c_str(), &st) < 0) {
        handle_get(client_sockfd, path + "/404.html");
        return;
    }
    //open request file.
    if ((f = open(file_name.c_str(), O_RDONLY)) < 0) { //BUG!! I writed ") < 0)){"
        perror("open file failed");
        close(client_sockfd);
        return;
    }
    //send headers.
    if(write(client_sockfd, HEADER.c_str(), HEADER.size()) < 0) {
        perror("response head写入失败.");
    }
    //send file to client.
    sendfile(client_sockfd, f, NULL, st.st_size + 1);
    close(f);
}

string HttpServer::get_time() {
    time_t t = time(nullptr);
    //asctime():transform date and time to broken-down time or ASCII.
    char *time = asctime(localtime(&t));
    string res(time);
    if (res[int(res.size()) - 1] == '\n')
        res = res.substr(0, int(res.size()) - 1);
    return res;
}

void HttpServer::handle_post(int client_sockfd, string buf) {
    size_t buf_index = 0;
    while (buf[buf_index++] != '/');
    string method;
    while (buf[buf_index++] != ' ') method += buf[buf_index - 1];
    cout << method << endl;
    try {
        if (method == "login") {

        }
    } catch (...) {
        cout << "post error..." << endl;
    }
}

int HttpServer::get_port() {
    return this->port;
}

int HttpServer::set_port(int new_port) {
    this->port = new_port;
    return this->port;
}

int HttpServer::get_backlog() {
    return this->backlog;
}

int HttpServer::set_backlog(int new_backlog) {
    this->backlog = new_backlog;
    return this->backlog;
}

void HttpServer::run() {

    server_sockfd = get_ipv4_socket_bind_and_listen();

    //创建epoll文件描述符
    epoll_fd = epoll_create1(0);//epoll_create1()相比epoll_create()，可以添加EPOLL_CLOEXEC等参数
    if (epoll_fd == -1) {
        perror("create epoll failed!\n");
        exit(1);
    }

    event.events = EPOLLIN;//EPOLLIN：只有当有数据写入时才会触发
    event.data.fd = server_sockfd;

    //将文件描述符添加到epoll实例epoll_fd中，注册监听事件
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sockfd, &event)) {//EPOLL_CTL_ADD:注册新的fd到epoll fd中
        perror("add server_sockfd to epoll failed\n");
        close(epoll_fd);
        exit(1);
    }

    while (true) {
        string method;

        int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);//等待事件产生，类似于select的调用
        if (event_count < 0) {
            perror("epoll_wait failed\n");
            continue;
        }
        for (int i = 0; i < event_count; i++) {
            int fd = events[i].data.fd;
            if (fd == server_sockfd) {
                //use socklen_t or in accept() use (socklen_t *)&socket_len.
                socklen_t socket_len = sizeof(struct sockaddr_in);
                memset(&client_address, 0, sizeof(client_address));

                //accept a client connect.
                int client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &socket_len);
                if (client_sockfd < 0) {
                    perror("accept client failed");
                    continue;
                }
                event.data.fd = client_sockfd;
                event.events = EPOLLIN;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &event)) {
                    perror("add client_sockfd to epoll failed\n");
                    continue;
                }
            } else {
                int client_sockfd = fd;
                memset(request_buf, '\0', sizeof(char) * N);
                int request_len = read(client_sockfd, request_buf, N - 1);//last is '\0'.
                string buf(request_buf);
                //cout << buf << endl;
                //cout << "****" << buf << "*****" <<endl;
                cout << "[" << get_time() << "] ";
                for (size_t buf_index = 0; buf_index < buf.size(); buf_index++) {
                    if (buf[buf_index] == '/')break;
                    method += buf[buf_index];
                }
                if (method.substr(0, 4) == "POST") {
                    cout << method;
                    handle_post(client_sockfd, buf);
                    continue;
                } else if (method.substr(0, 3) == "GET") {
                    string file_name = get_request_file_name(buf);
                    if (file_name.substr(0, 4) != "dir/") {
                        file_name = path + '/' + file_name;
                    }
                    handle_get(client_sockfd, file_name);
                }
                close(client_sockfd);
            }
        }
    }

}