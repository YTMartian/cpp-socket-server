#include "head.h"
#include "HttpServer.h"

void sig_pipe(int signo) {
    logger->warn("捕获信号:SIGPIPE({})", signo);
}

void sig_segv(int signo) {
    logger->warn("捕获信号:SIGSEGV({})", signo);
}

int main(int argc, char **argv) {

    //绑定信号处理函数
    if ((signal(SIGPIPE, sig_pipe) == SIG_ERR)) {
        spdlog::error("绑定信号处理函数失败:{}({})", __FILE__, __LINE__);
        exit(1);
    }
    
    if ((signal(SIGSEGV, sig_segv) == SIG_ERR)) {
        spdlog::error("绑定信号处理函数失败:{}({})", __FILE__, __LINE__);
        exit(1);
    }

    int port = DEFAULT_PORT;
    int backlog = DEFAULT_BACKLOG;
    bool use_redis = false;
    bool use_multithread = true;

    char c;
    //处理长参数:https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html#Getopt-Long-Option-Example
    while ((c = getopt(argc, argv, "p:b:c:t:")) != -1) {//处理参数
        if (c == 'p') {//port
            port = atoi(optarg);
            if (to_string(port) != optarg) {
                spdlog::error("非数字: {}", optarg);
                exit(1);
            }
        } else if (c == 'b') {//backlog
            backlog = atoi(optarg);
            if (to_string(backlog) != optarg) {
                spdlog::error("非数字: {}", optarg);
                exit(1);
            }
        } else if (c == 'c') {
            string s = string(optarg);
            if (s == "true") {
                use_redis = true;
            } else if (s == "false") {
                use_redis = false;
            } else {
                spdlog::error("非法参数: {}", optarg);
                exit(1);
            }
        } else if (c == 't') {
            string s = string(optarg);
            if (s == "true") {
                use_multithread = true;
            } else if (s == "false") {
                use_multithread = false;
            } else {
                spdlog::error("非法参数: {}", optarg);
                exit(1);
            }
        } else if (c == '?') {//未知参数
            spdlog::error("未知参数");
            spdlog::error(
                    "./server [-p <端口号>] [-b <backlog>] [-c <true/false(是否使用缓存，默认不使用)>] [-t <true/false(是否使用多线程，默认使用)>]");
            exit(1);
        }
    }
    if (optind != argc) {
        spdlog::error(
                "./server [-p <端口号>] [-b <backlog>] [-c <true/false(是否使用缓存，默认不使用)>] [-t <true/false(是否使用多线程，默认使用)>]");
        exit(1);
    }

    if (use_redis) {
        logger->info("启用redis.");
        spdlog::info("启用redis.");
    }
    shared_ptr<HttpServer> server(new HttpServer(use_redis, use_multithread));
    server->set_port(port);
    server->set_backlog(backlog);
    server->run();

    return 0;
}
