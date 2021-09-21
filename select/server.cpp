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
    socklen_t socket_len = sizeof(struct sockaddr_in);

    fd_set rset; //一组文件描述符
    FD_ZERO(&rset);//清空该组文件描述符
    struct timeval timeout;//设置超时
    timeout.tv_sec = 7;//秒
    timeout.tv_usec = 1000;//毫秒

    while(true) {
        string method;

        FD_SET(server_sockfd, &rset);//将一个给定的文件描述符从集合中删除
        //select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
        //nfds:一个整数值，是指集合中所有文件描述符的范围，即所有文件描述符的最大值加1
        //fd_set *readfds---->用来检查一组可读性的文件描述符。
        //fd_set *writefds---->用来检查一组可写性的文件描述符。
        //fd_set *exceptfds---->用来检查文件文件描述符是否异常
        //sreuct timeval *timeout--->是一个时间结构体，用来设置超时时间,对阻塞操作则为NULL
        int select_id = select(server_sockfd + 1, &rset, NULL, NULL, &timeout);
        if(select_id == -1) {
            perror("select failed!\n");
            break;
        } else if(select_id == 0) {
            perror("timeout!\n");
            continue;
        }

        if(FD_ISSET(server_sockfd, &rset)) {//将一个文件描述符添加到一个指定的文件描述符集合中
            //use socklen_t or in accept() use (socklen_t *)&socket_len.
            memset(&client_address, 0, sizeof(client_address));

            //accept a client connect.
            int client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &socket_len);
            if(client_sockfd < 0) {
                perror("accept client failed");
                continue;
            }
            //get GET request from client.
            memset(request_buf, '\0', sizeof(char) * N);
            read(client_sockfd, request_buf, N - 1);//last is '\0'.
            string buf(request_buf);
            //cout << buf << endl;
            //cout << "****" << buf << "*****" <<endl;
            cout << "[" << get_time() << "] ";
            for(int buf_index = 0; buf_index < buf.size(); buf_index++) {
                if(buf[buf_index] == '/')break;
                method += buf[buf_index];
            }
            if(method.substr(0, 4) == "POST") {
                cout <<  method;
                handle_post(client_sockfd, buf);
            }
            string file_name = get_request_file_name(buf);
            if(file_name.substr(0, 4) != "dir/") {
                file_name = path + '/' + file_name;
            }
            // if(file_name == "") {
            //     continue;
            // }

            handle_get(client_sockfd, file_name);
        }
    }

    return 0;
}
