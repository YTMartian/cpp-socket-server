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
    string path = "www";
    int backlog = 128;//max contain un-handle connects.
    int server_sockfd = get_ipv4_socket_bind_and_listen(&server_address, port, backlog);

    //char request_buf[N];
    vector<pthread_t>threads(MAX_THREAD);
    int thread_index = 0;

    while(true) {
        //use socklen_t or in accept() use (socklen_t *)&socket_len.
        socklen_t socket_len = sizeof(struct sockaddr_in);
        memset(&client_address, 0, sizeof(client_address));

        //accept a client connect.
        int client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &socket_len);
        if(client_sockfd < 0) {
            perror("accept client failed");
            continue;
        }
        if(pthread_create(&threads[thread_index++], NULL, socket_thread, &client_sockfd) != 0) {
            cout << "create thread failed.\n";
            continue;
        }
        if(thread_index >= MAX_THREAD) {
            thread_index = 0;
            while(thread_index < MAX_THREAD) {
                pthread_join(threads[thread_index], NULL);
            }
            thread_index = 0;
        }
    }

    return 0;
}
