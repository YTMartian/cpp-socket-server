//
// Created by tim on 2021/9/26.
//

#include "PostProcessor.h"

PostProcessor::PostProcessor() {
    this->method = llhttp_method::HTTP_POST;
    keep_alive = false;
}

PostProcessor::~PostProcessor() = default;

void PostProcessor::set_client_sockfd(int new_fd) {
    client_sockfd = new_fd;
}

void PostProcessor::set_message_body(const string &body) {
    if (!json_reader.parse(body, message_body, false)) {
        logger->error("post json数据解析失败:{}({})", __FILE__, __LINE__);
        throw runtime_error("post设置body失败");
    }
}

llhttp_method PostProcessor::get_method() {
    return this->method;
}

void PostProcessor::run() {

}