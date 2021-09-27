//
// Created by tim on 2021/9/25.
//

#ifndef EPOLL_HEAD_H
#define EPOLL_HEAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>//close(), read(), getopt(), alarm()
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
#include <jsoncpp/json/json.h>
#include <sys/epoll.h>//epoll_create1(), epoll_ctl(), struct epoll_event
#include "llhttp.h"
using namespace std;


const string HEADER = "HTTP/1.1 200 OK\r\n\r\n";
const int N = 4096;
const string path = "../www";
const int MAX_EVENTS = 100;
const bool IS_TEST = false;//测试的时候不输出其它信息
const int DEFAULT_PORT = 12345;
const int DEFAULT_BACKLOG = 128;//max contain un-handle connects.

#endif //EPOLL_HEAD_H
