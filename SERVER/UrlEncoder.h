//
// Created by tim on 2021/9/27.
//

#ifndef SERVER_URLENCODER_H
#define SERVER_URLENCODER_H

#include "head.h"

class UrlEncoder {
private:
    unsigned char to_hex(unsigned char x);

    unsigned char from_hex(unsigned char x);

public:
    UrlEncoder();

    ~UrlEncoder();

    string url_encode(const string &str) noexcept(false);

    string url_decode(const string &str) noexcept(false);
};


#endif //SERVER_URLENCODER_H
