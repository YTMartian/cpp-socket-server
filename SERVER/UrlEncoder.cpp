//
// Created by tim on 2021/9/27.
//

#include "UrlEncoder.h"

UrlEncoder::UrlEncoder() {

}

UrlEncoder::~UrlEncoder() {

}

unsigned char UrlEncoder::to_hex(unsigned char x) {
    return x > 9 ? x + 55 : x + 48;
}

unsigned char UrlEncoder::from_hex(unsigned char x) {
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else
        assert(0);
    return y;
}

string UrlEncoder::url_encode(const string &str)noexcept(false) {
    try{
        string strTemp;
        size_t length = str.length();
        for (size_t i = 0; i < length; i++) {
            if (isalnum((unsigned char) str[i]) ||
                (str[i] == '-') ||
                (str[i] == '_') ||
                (str[i] == '.') ||
                (str[i] == '~'))
                strTemp.push_back(str[i]);
            else if (str[i] == ' ')
                strTemp.push_back('+');
            else {
                strTemp.push_back('%');
                strTemp.push_back(to_hex((unsigned char) str[i] >> 4));
                strTemp.push_back(to_hex((unsigned char) str[i] % 16));
            }
        }
        return strTemp;
    } catch(exception &e) {
        logger->error("错误： {} 第{}行 {}", __FILE__, __LINE__, e.what());
        throw e;
    }
}

string UrlEncoder::url_decode(const string &str)noexcept(false) {
    try{
        string strTemp = "";
        size_t length = str.length();
        for (size_t i = 0; i < length; i++) {
            if (str[i] == '+') strTemp.push_back(' ');
            else if (str[i] == '%') {
                assert(i + 2 < length);
                unsigned char high = from_hex((unsigned char) str[++i]);
                unsigned char low = from_hex((unsigned char) str[++i]);
                strTemp.push_back(high * 16 + low);
            } else strTemp.push_back(str[i]);
        }
        return strTemp;
    } catch(exception &e) {
        logger->error("错误： {} 第{}行 {}", __FILE__, __LINE__, e.what());
        throw e;
    }
}