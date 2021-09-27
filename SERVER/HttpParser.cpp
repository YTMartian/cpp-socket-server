//
// Created by tim on 2021/9/25.
//

#include "HttpParser.h"

HttpParser::HttpParser() {

}

void HttpParser::parse(char *request, int request_len) {
    for (int i = 0; i < request_len; i++) cout << request[i];
    cout << endl;
}

string HttpParser::get_method() {
    return this->method;
}