//
// Created by tim on 2021/9/26.
//

#include "PostProcessor.h"

PostProcessor::PostProcessor() {

}

PostProcessor::~PostProcessor() {

}

void PostProcessor::set_client_sockfd(int new_fd) {
    client_sockfd = new_fd;
}

void PostProcessor::run() {

}