//
// Created by tim on 2021/9/25.
//

#include "HttpServer.h"

HttpServer::HttpServer(bool use_redis) {
    port = DEFAULT_PORT;
    backlog = DEFAULT_BACKLOG;
    request_buf = new char[N];
    server_address = new struct sockaddr_in;
    client_address = new struct sockaddr_in;
    http_parser_settings = new llhttp_settings_t;
    processor = nullptr;
    USE_REDIS = use_redis;
    if (USE_REDIS) {
        redis = new Redis();
        try {
            redis->connect();
            logger->info("redis连接成功.");
            spdlog::info("redis连接成功.");
        } catch (exception &e) {
            USE_REDIS = false;
            delete redis;
            redis = nullptr;
        }
    } else redis = nullptr;
    /**********************************************************************************/
    /*当部署到云服务器时，把以下的初始化llhttp的代码放到enum llhttp_errno err = llhttp_execute(&http_parser, request_buf, request_len);前面，否则llhttp只会解析一次，具体原因未知*/
    llhttp_settings_init(http_parser_settings);
    //设置回调函数
    set_callbacks(http_parser_settings);
    llhttp_init(&http_parser, HTTP_REQUEST, http_parser_settings);
    http_parser.data = this;//一定要先绑定，否则reinterpret_cast时有问题，找了半天错
    /**********************************************************************************/
}

HttpServer::~HttpServer() {
    delete[] request_buf;
    delete server_address;
    delete client_address;
    delete redis;//nullptr也可delete
}

int HttpServer::get_ipv4_socket_bind_and_listen() {
    int server_fd;
    server_address->sin_family = AF_INET;//set socket domain.
    server_address->sin_port = htons(port);//host bytes-order to network bytes-order.
    server_address->sin_addr.s_addr = INADDR_ANY;// ip address is 0.0.0.0

    //建立socket.
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        logger->error("socket建立失败:{}({})", __FILE__, __LINE__);
        spdlog::error("socket建立失败:{}({})", __FILE__, __LINE__);
        exit(EXIT_FAILURE);//exit(1)
    }
    if ((bind(server_fd, (struct sockaddr *) server_address, sizeof(struct sockaddr))) < 0) {
        logger->error("socket建立失败,端口{}已占用", port);
        spdlog::error("socket建立失败,端口{}已占用", port);
        exit(EXIT_FAILURE);
    }
    logger->info("socket建立成功.");
    spdlog::info("socket建立成功.");
    //监听
    if (listen(server_fd, backlog) < 0) {
        logger->error("启动监听失败.");
        spdlog::error("启动监听失败.");
        exit(EXIT_FAILURE);
    }
    logger->info("监听端口:{}", port);
    spdlog::info("监听端口:{}", port);
    return server_fd;
}

int HttpServer::set_port(int new_port) {
    this->port = new_port;
    return this->port;
}

int HttpServer::set_backlog(int new_backlog) {
    this->backlog = new_backlog;
    return this->backlog;
}

int HttpServer::on_message_begin() {
    this->processor = nullptr;
    this->head_fields.clear();
    this->head_values.clear();
    return 0;
}

int HttpServer::on_url(llhttp_t *parser, const char *at, size_t length) {
    try {
        processor = ProcessorFactory::get_processor((llhttp_method) (parser->method));
        if (processor == nullptr) return -1;
        string url = string(at, length);
        url = this->url_encoder.url_decode(url);
        url = url.substr(1, url.size() - 1);
        processor->set_url(url);
        processor->set_redis(this->redis);
        processor->set_use_redis(USE_REDIS);
        return 0;
    } catch (exception &e) {
        logger->error("错误：{}({})", __FILE__, __LINE__);
        return -1;
    }
}

int HttpServer::on_header_field(const char *at, size_t length) {
    string field = string(at, length);
    transform(field.begin(), field.end(), field.begin(), ::tolower);//全部转为小写字母
    this->head_fields.emplace_back(field);
    return 0;
}

int HttpServer::on_header_value(const char *at, size_t length) {
    string value = string(at, length);
    transform(value.begin(), value.end(), value.begin(), ::tolower);//全部转为小写字母
    this->head_values.emplace_back(value);
    return 0;
}

int HttpServer::on_headers_complete() {
    if (this->head_fields.size() != this->head_values.size()) {
        logger->error("head的field({})和value({})数量不匹配", this->head_fields.size(), this->head_values.size());
        return -1;
    }
    size_t len = this->head_fields.size();
    for (size_t i = 0; i < len; i++) {
        this->processor->add_head(this->head_fields[i], this->head_values[i]);
    }
    return 0;
}

int HttpServer::on_body(const char *at, size_t len) {
    string body = string(at, len);
    try {
        processor->set_message_body(body);
    } catch (exception &e) {
        return -1;
    }
    return 0;
}

int HttpServer::on_message_complete() {
    if (processor == nullptr) return -1;
    return 0;
}

void HttpServer::set_callbacks(llhttp_settings_t *settings) {
    settings->on_message_begin = HttpServer::callback_on_message_begin;
    settings->on_status = nullptr;
    settings->on_chunk_header = nullptr;
    settings->on_chunk_complete = nullptr;
    settings->on_url = HttpServer::callback_on_url;
    settings->on_header_field = HttpServer::callback_on_header_field;
    settings->on_header_value = HttpServer::callback_on_header_value;
    settings->on_headers_complete = HttpServer::callback_on_headers_complete;
    settings->on_body = HttpServer::callback_on_body;
    settings->on_message_complete = HttpServer::callback_on_message_complete;
}

void HttpServer::run() {

    server_sockfd = get_ipv4_socket_bind_and_listen();

    //创建epoll文件描述符
    epoll_fd = epoll_create1(0);//epoll_create1()相比epoll_create()，可以添加EPOLL_CLOEXEC等参数
    if (epoll_fd == -1) {
        logger->error("创建epoll失败");
        exit(1);
    }

    event.events = EPOLLIN;//EPOLLIN：只有当有数据写入时才会触发
    event.data.fd = server_sockfd;

    //将文件描述符添加到epoll实例epoll_fd中，注册监听事件
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sockfd, &event)) {//EPOLL_CTL_ADD:注册新的fd到epoll fd中
        logger->error("server_sockfd添加到epoll失败");
        close(epoll_fd);
        exit(1);
    }
    int client_sockfd;
    ssize_t request_len;

    while (true) {
        int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);//等待事件产生，类似于select的调用
        if (event_count < 0) {
            logger->error("epoll_wait失败: {}({})", __FILE__, __LINE__);
            continue;
        }
        for (int i = 0; i < event_count; i++) {
            int fd = events[i].data.fd;
            if (fd == server_sockfd) {
                //use socklen_t or in accept() use (socklen_t *)&socket_len.
                socklen_t socket_len = sizeof(struct sockaddr_in);
                memset(&client_address, 0, sizeof(client_address));

                //accept a client connect.
                client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &socket_len);
                if (client_sockfd < 0) {
                    logger->error("accept client失败:{}({})", __FILE__, __LINE__);
                    continue;
                }
                event.data.fd = client_sockfd;
                event.events = EPOLLIN | EPOLLET;//边缘触发
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &event)) {
                    logger->error("client_sockfd添加到epoll失败");
                    continue;
                }
            } else {
                client_sockfd = fd;
                memset(request_buf, '\0', sizeof(char) * N);
                if ((request_len = read(client_sockfd, request_buf, N - 1)) < 0) {//last is '\0'.
                    if(request_len < 0) logger->error("client读取失败: {}({})", __FILE__, __LINE__);
                    close(client_sockfd);
                    continue;
                }
                if (request_len == N - 1) {
                    logger->error("请求过大(最大长度{}): {}({})", N, __FILE__, __LINE__);
                    close(client_sockfd);
                    continue;
                }
                enum llhttp_errno err = llhttp_execute(&http_parser, request_buf, request_len);
                if (err == HPE_OK && processor != nullptr) {
                    try {
                        processor->set_client_sockfd(client_sockfd);
                        processor->run();
                    } catch (exception &e) {
                        logger->error("请求执行失败: {}({})", __FILE__, __LINE__);
                        close(client_sockfd);
                    }
                } else {
                    logger->error("http解析失败: {} {}", string(llhttp_errno_name(err)), string(http_parser.reason));
                    close(client_sockfd);
                }
            }
        }
    }
}