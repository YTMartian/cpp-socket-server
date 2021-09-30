//
// Created by tim on 2021/9/26.
//

#include "GetProcessor.h"

GetProcessor::GetProcessor() {
    this->method = llhttp_method::HTTP_GET;
}

GetProcessor::~GetProcessor() = default;

void GetProcessor::set_client_sockfd(int new_fd) {
    this->client_sockfd = new_fd;
}

llhttp_method GetProcessor::get_method() {
    return this->method;
}

int GetProcessor::set_message_body(const string &body) {

}

void GetProcessor::send_file(const string &file_name) {
    string name = DIR_PATH + file_name;
    struct stat st{};
    int f = -1;
    string response_head;
    //首先判断该文件是否在DIR_PATH里，否则没有权限读取

    if (stat(name.c_str(), &st) < 0) {//文件不存在
        if ((write(client_sockfd, RESPONSE_404.c_str(), RESPONSE_404.size())) < 0) {
            logger->error("获取文件{}信息失败:{}({})", name, __FILE__, __LINE__);
        }
        logger->error("文件不存在:{}", name);
        return;
    } else if (!S_ISREG(st.st_mode) || !(S_IRUSR & st.st_mode)) {
        /* S_ISREG:是否是一个常规文件
         * S_IRUSR:所有者拥有读权限
         */
        if ((write(client_sockfd, RESPONSE_403.c_str(), RESPONSE_403.size())) < 0) {
            logger->error("获取文件{}信息失败:{}({})", name, __FILE__, __LINE__);
        }
        logger->error("文件不可读:{}", name);
        return;
    } else {
        response_head = RESPONSE_200;
        response_head += "Accept-Ranges: bytes\r\n";
        response_head += "Content-Length:" + to_string(st.st_size) + "\r\n";
        string type;
        for (size_t i = file_name.size() - 1; ~i; i--) {
            type.push_back(file_name[i]);
            if (file_name[i] == '.') break;
        }
        reverse(type.begin(), type.end());
        if (CONTENT_TYPE.find(type) != CONTENT_TYPE.end()) {
            response_head += "Content-Type: " + CONTENT_TYPE.at(type) + "\r\n";
        }
        response_head += "\r\n";//注意是http报文头和数据之间还有个空行
        if ((write(client_sockfd, response_head.c_str(), response_head.size())) < 0) {
            logger->error("发送response头失败:{}({})", name, __FILE__, __LINE__);
            return;
        }
    }
    //打开文件
    if ((f = open(name.c_str(), O_RDONLY, 0)) < 0) { //BUG!! I writed ") < 0)){"
        logger->error("打开文件{}错误:{}({})", name, __FILE__, __LINE__);
        if ((write(client_sockfd, RESPONSE_500.c_str(), RESPONSE_500.size())) < 0) {
            logger->error("发送response头失败:{}({})", name, __FILE__, __LINE__);
        }
        return;
    }
    //发送文件
    try{
        if ((sendfile(client_sockfd, f, NULL, st.st_size)) < 0) {
            logger->error("发送文件失败:{}({})", name, __FILE__, __LINE__);
        }
        close(f);
    } catch(exception &e) {
        logger->error("发送文件失败:{}({})", name, __FILE__, __LINE__);
    }
}

void GetProcessor::run() {
    logger->info("{} {}", llhttp_method_name(this->method), this->url);
    //判断url是方法还是获取一个文件,如果以'/'结尾的则说明是方法
    if (!url.empty() && url.back() != '/') {//是文件
        send_file(url);
    } else {

    }
    close(client_sockfd);
}