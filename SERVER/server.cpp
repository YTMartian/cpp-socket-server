#include "head.h"
#include "HttpServer.h"


int main(int argc, char **argv) {

    shared_ptr<HttpServer> server(new HttpServer());

    char c;
    while ((c = getopt(argc, argv, "p:b:")) != -1) {//处理参数
        if (c == 'p') {//port
            int port = atoi(optarg);
            if (to_string(port) != optarg) {
                logger->error("非数字: {}", optarg);
                exit(1);
            }
            server->set_port(port);
        } else if (c == 'b') {//backlog
            int backlog = atoi(optarg);
            if (to_string(backlog) != optarg) {
                logger->error("非数字: {}", optarg);
                exit(1);
            }
            server->set_backlog(backlog);
        } else if (c == '?') {//未知参数
            exit(1);
        }
    }
    if (optind != argc) {
        spdlog::info("使用方法: ./server [-p <端口号>] [-b <backlog>]\n");
        exit(1);
    }

    server->run();

    return 0;
}
