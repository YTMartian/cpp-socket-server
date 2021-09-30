#include "http.h"


int main(int argc, char **argv) {

    if(argc > 2) {
        perror("Too many parameters");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int port = 8000;//default port.
    if(argc == 2)port = atoi(argv[1]);
    int backlog = 128;//max contain un-handle connects.
    int server_sockfd = get_ipv4_socket_bind_and_listen(&server_address, port, backlog);

    char request_buf[N];

    //创建epoll文件描述符
    int epoll_fd = epoll_create1(0);//epoll_create1()相比epoll_create()，可以添加EPOLL_CLOEXEC等参数
    if(epoll_fd == -1) {
        perror("create epoll failed!\n");
        return 1;
    }
    struct epoll_event event, events[MAX_EVENTS];//events:即wait event
    event.events = EPOLLIN;//EPOLLIN：只有当有数据写入时才会触发
    event.data.fd = server_sockfd;

    //将文件描述符添加到epoll实例epoll_fd中，注册监听事件
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sockfd, &event)) {//EPOLL_CTL_ADD:注册新的fd到epoll fd中
        perror("add server_sockfd to epoll failed\n");
        close(epoll_fd);
        return 1;
    }

    while(true) {
        string method;

        int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);//等待事件产生，类似于select的调用
        if(event_count < 0) {
            perror("epoll_wait failed\n");
            continue;
        }
        for(int i = 0; i < event_count; i++) {
            int fd = events[i].data.fd;
            if(fd == server_sockfd) {
                //use socklen_t or in accept() use (socklen_t *)&socket_len.
                socklen_t socket_len = sizeof(struct sockaddr_in);
                memset(&client_address, 0, sizeof(client_address));

                //accept a client connect.
                int client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &socket_len);
                if(client_sockfd < 0) {
                    perror("accept client failed");
                    continue;
                }
                event.data.fd = client_sockfd;
                event.events = EPOLLIN;
                if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &event)) {
                    perror("add client_sockfd to epoll failed\n");
                    continue;
                }
            } else {
                int client_sockfd = fd;
                memset(request_buf, '\0', sizeof(char) * N);
                read(client_sockfd, request_buf, N - 1);//last is '\0'.
                string buf(request_buf);
                //cout << buf << endl;
                //cout << "****" << buf << "*****" <<endl;
                if(!IS_TEST) cout << "[" << get_time() << "] ";
                for(int buf_index = 0; buf_index < buf.size(); buf_index++) {
                    if(buf[buf_index] == '/')break;
                    method += buf[buf_index];
                }
                if(method.substr(0, 4) == "POST") {
                    if(!IS_TEST) cout << method;
                    handle_post(client_sockfd, buf);
                    continue;
                } else if(method.substr(0, 3) == "GET") {
                    string file_name = get_request_file_name(buf);
                    if(file_name.substr(0, 4) != "dir/") {
                        file_name = path + '/' + file_name;
                    }

                    handle_get(client_sockfd, file_name);
                }
                close(client_sockfd);
            }

        }

    }

    return 0;
}
