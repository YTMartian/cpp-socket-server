#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>//struct sockaddr_in
#include <iostream>
#include <fcntl.h>//open()
#include <time.h>
#include <sys/stat.h>//stat()
#include <sys/sendfile.h>//sendfile()
#include <numeric>
#include <vector>
#include <pthread.h>
#include <utility>
#include <sqlite3.h>
#include <assert.h>
#include <fstream>
#include <dirent.h>
#include <sstream>

using namespace std;
//response headers.
const string HEADER = "HTTP/1.1 200 OK\r\n\r\n";
const int N = 4096;
const int MAX_THREAD = 100;
const string path = "../../www/";

//set AF_INET socket, bind it and listen it.
int get_ipv4_socket_bind_and_listen(struct sockaddr_in *server_address, int port, int backlog);
//analyze the request string and get the request file name.
string get_request_file_name(string buf);
void handle_get(int client_sockfd, string file_name);
void handle_post(int client_sockfd, string buf);
string get_time();
void *socket_thread(void *arg);
