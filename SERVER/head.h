//
// Created by tim on 2021/9/25.
//
#ifndef SERVER_HEAD_H
#define SERVER_HEAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unistd.h>//close(), read(), getopt(), alarm()
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>//socket()、setsockopt()
#include <netinet/in.h>//struct sockaddr_in
#include <fcntl.h>//open()
#include <time.h>
#include <sys/stat.h>//stat()
#include <sys/sendfile.h>//sendfile()
#include <numeric>
#include <vector>
#include <pthread.h>
#include <utility>
#include <assert.h>
#include <fstream>
#include <dirent.h>
#include <sstream>
#include <jsoncpp/json/json.h>
#include <sys/epoll.h>//epoll_create1(), epoll_ctl(), struct epoll_event
#include "llhttp.h"
#include <map>
#include <exception>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <hiredis/hiredis.h>
#include <netinet/tcp.h>
#include <csignal>

using namespace std;


#define N 4096
const string DIR_PATH = "../www/";
const string LOG_DIR = "../logs/";
#define MAX_EVENTS 256
#define DEFAULT_PORT  12345
#define DEFAULT_BACKLOG 128//max contain un-handle connects.
#define KEEPALIVE_TIMEOUT 500//ms
const string RESPONSE_200 = "HTTP/1.1 200 OK\r\n";
const string RESPONSE_403 = "HTTP/1.1 403 Forbidden\r\n\r\n";//注意是http报文头和数据之间还有个空行
const string RESPONSE_404 = "HTTP/1.1 404 Not Found\r\n\r\n";
const string RESPONSE_500 = "HTTP/1.1 500 Internal Server Error\r\n\r\n";

// 创建一个最大LOG_MAX_SIZE MB大小和LOG_MAX_FILES个滚动文件的滚动日志
#define LOG_MAX_SIZE (1048576 * 10)
#define LOG_MAX_FILES 5
//声明为inline，否则会报multiple definition of的编译错误
//inline auto logger = spdlog::rotating_logger_mt("my_log", LOG_DIR + "rotating.txt", LOG_MAX_SIZE, LOG_MAX_FILES);
inline auto logger = make_shared<spdlog::logger>("my_log", make_shared<spdlog::sinks::stdout_color_sink_mt>());//输出到屏幕
//static bool USE_REDIS = true;
#define CACHE_FILE_MAX_SIZE (1024 * 100)//将不超过CACHE_FILE_MAX_SIZE字节的文件缓存
inline char FILE_BUF[N];

/******************
定义http mime
******************/
//const型的map不能通过[]获取值，而要用map.at()函数
inline map<string, string> CONTENT_TYPE = {
        {".aac",    "audio/aac"},
        {".abw",    "application/x-abiword"},
        {".arc",    "application/x-freearc"},
        {".avi",    "video/x-msvideo"},
        {".azw",    "application/vnd.amazon.ebook"},
        {".bin",    "application/octet-stream"},
        {".bmp",    "image/bmp"},
        {".bz",     "application/x-bzip"},
        {".bz2",    "application/x-bzip2"},
        {".cda",    "application/x-cdf"},
        {".csh",    "application/x-csh"},
        {".css",    "text/css"},
        {".csv",    "text/csv"},
        {".doc",    "application/msword"},
        {".docx",   "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
        {".eot",    "application/vnd.ms-fontobject"},
        {".epub",   "application/epub+zip"},
        {".gz",     "application/gzip"},
        {".gif",    "image/gif"},
        {".htm",    "text/html"},
        {".html",   "text/html"},
        {".ico",    "image/vnd.microsoft.icon"},
        {".ics",    "text/calendar"},
        {".jar",    "application/java-archive"},
        {".jpg",    "image/jpeg"},
        {".jpeg",   "image/jpeg"},
        {".js",     "text/javascript"},
        {".json",   "application/json"},
        {".jsonld", "application/ld+json"},
        {".midi",   "audio/x-midi"},
        {".mid",    "audio/midi"},
        {".mjs",    "text/javascript"},
        {".mp3",    "audio/mpeg"},
        {".mp4",    "video/mp4"},
        {".mpeg",   "video/mpeg"},
        {".mpkg",   "application/vnd.apple.installer+xml"},
        {".odp",    "application/vnd.oasis.opendocument.presentation"},
        {".ods",    "application/vnd.oasis.opendocument.spreadsheet"},
        {".odt",    "application/vnd.oasis.opendocument.text"},
        {".oga",    "audio/ogg"},
        {".ogv",    "video/ogg"},
        {".ogx",    "application/ogg"},
        {".opus",   "audio/opus"},
        {".otf",    "font/otf"},
        {".png",    "image/png"},
        {".pdf",    "application/pdf"},
        {".php",    "application/x-httpd-php"},
        {".ppt",    "application/vnd.ms-powerpoint"},
        {".pptx",   "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
        {".rar",    "application/vnd.rar"},
        {".rtf",    "application/rtf"},
        {".sh",     "application/x-sh"},
        {".svg",    "image/svg+xml"},
        {".swf",    "application/x-shockwave-flash"},
        {".tar",    "application/x-tar"},
        {".tif",    "image/tiff"},
        {".tiff",   "image/tiff"},
        {".ts",     "video/mp2t"},
        {".ttf",    "font/ttf"},
        {".txt",    "text/plain"},
        {".vsd",    "application/vnd.visio"},
        {".wav",    "audio/wav"},
        {".weba",   "audio/webm"},
        {".webm",   "video/webm"},
        {".webp",   "image/webp"},
        {"woff",    "font/woff"},
        {".woff2",  "font/woff2"},
        {".xhtml",  "application/xhtml+xml"},
        {".xls",    "application/vnd.ms-excel"},
        {".xlsx",   "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
        {".xml",    "application/xml"},
        {".xul",    "application/vnd.mozilla.xul+xml"},
        {".zip",    "application/zip"},
        {".3gp",    "video/3gpp"},
        {".3g2",    "video/3gpp2"},
        {".7z",     "application/x-7z-compressed"}
};


#endif //SERVER_HEAD_H
