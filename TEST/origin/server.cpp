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

    while(true) {
        string method;

        //use socklen_t or in accept() use (socklen_t *)&socket_len.
        socklen_t socket_len = sizeof(struct sockaddr_in);
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

    return 0;
}
