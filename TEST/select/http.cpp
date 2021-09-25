#include "http.h"


int get_ipv4_socket_bind_and_listen(struct sockaddr_in *server_address, int port, int backlog) {
    int server_fd;
    server_address->sin_family = AF_INET;//set socket domain.
    server_address->sin_port = htons(port);//host bytes-order to network bytes-order.
    server_address->sin_addr.s_addr = INADDR_ANY;// ip address is 0.0.0.0

    //establish a socket.
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("establish socket failed");
        exit(EXIT_FAILURE);//exit(1)
    }
    cout << "server socket: " << server_fd << endl;
    //bind server socket to server address.
    if((bind(server_fd, (struct sockaddr *)server_address, sizeof(struct sockaddr))) < 0) {
        perror("socket bind failed");
        exit(EXIT_FAILURE);
    }
    cout << "socket bind successful." << endl;
    //listen to connection from clients.
    if(listen(server_fd, backlog) < 0) {
        perror("start listening failed");
        exit(EXIT_FAILURE);
    }
    cout << "listening port: " << port << endl;
    return server_fd;
}

string get_request_file_name(string buf) {
    //the request buf is like "GET /index.html HTTP/1.1 ... ..."
    int buf_index = 0;
    string info;
    while(buf_index < buf.size() && buf[buf_index++] != '/') info += buf[buf_index - 1];
    if(buf_index - 1 == buf.size() || buf[buf_index - 1] != '/')return "";//illegal request.
    string file_name;
    //extract request file from the request buf.
    while(buf[buf_index++] != ' ') file_name += buf[buf_index - 1];
    cout << info + file_name << endl;
    return file_name;
}

void handle_get(int client_sockfd, string file_name) {
    struct stat st;
    int f;
    //if file not find then return 404.
    if(stat(file_name.c_str(), &st) < 0) {
        handle_get(client_sockfd, path + "/404.html");
        return;
    }
    //open request file.
    if((f = open(file_name.c_str(), O_RDONLY)) < 0) { //BUG!! I writed ") < 0)){"
        perror("open file failed");
        close(client_sockfd);
        return;
    }
    //send headers.
    write(client_sockfd, HEADER.c_str(), HEADER.size());
    //send file to client.
    sendfile(client_sockfd, f, NULL, st.st_size + 1);
    close(f);

    // close(client_sockfd);//must close.
}

string get_time() {
    time_t t = time(nullptr);
    //asctime():transform date and time to broken-down time or ASCII.
    char *time = asctime(localtime(&t));
    string res(time);
    if(res[int(res.size()) - 1] == '\n')
        res = res.substr(0, int(res.size()) - 1);
    return res;
}

void handle_post(int client_sockfd, string buf) {
    int buf_index = 0;
    while(buf[buf_index++] != '/');
    string method;
    while(buf[buf_index++] != ' ') method += buf[buf_index - 1];
    cout << method << endl;
    int num;
    try {
        if(method == "login") {

        }
    } catch(...) {
        cout << "post error..." << endl;
    }
    // close(client_sockfd);
}
