CPP socket编程实现web后台服务器。

#### 功能

* [x] GET请求
* [x] POST请求
* [ ] HEAD请求
* [x] 状态码： 200、403、404、500
* [x] 日志记录
* [x] 高并发，最大支持 并发量
* [x] json格式解析（jsoncpp）
* [x] http报文解析（llhttp）
* [x] redis缓存
* [x] url编解析
* [x] 滚动日志记录（spdlog）

#### 环境

- 安装g++：`sudo apt install g++`
- 安装jsoncpp：`sudo apt install libjsoncpp-dev`
- 安装redis：`sudo apt install redis-server`
- 安装spdlog：`sudo apt install libspdlog-dev`
- 安装llhttp：`git clone https://github.com/nodejs/llhttp.git && cd llhttp && sudo apt install npm && sudo npm install typescript && sudo apt install clang && sudo make && sudo make install && sudo cp build/libllhttp.so build/llhttp.h <makefile所在目录>`

#### 运行

1. 进入里, 清除生成的.o文件 : `make clean`
2. 执行make命令：`make`(debug：`make debug=true`, `gdb ./server`后run，或使用valgrind调试`valgrind ./server`)
3. 运行服务端: `./server [-p <端口号>] [-b <backlog>]`,  默认端口12345，也可指定端口，如：./server -p 6666
4. 性能测试： 在`TEST/` 下有`multithreading/`、`origin/`、`select/`和`epoll/`四个文件夹分别对多线程、原始单路IO、select和epoll进行get性能测试，get的文件存放到`www/`里，编译运行后在其它窗口使用ab工具进行性能测试 (sudo apt-get install apache2-utils)

#### 最大线程数

1. 查看：编译：g++ test_max_thread.cpp -pthread test.o 运行：./test.o
2. 查看：cat /proc/sys/kernel/threads-max
3. 查看：sysctl kernel.threads-max
4. 修改：sudo sysctl -w kernel.threads-max=102400

#### 测试

