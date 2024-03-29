﻿Unix/Linux下的高并发高容错服务器示例[cpp-socket-server
](https://github.com/YTMartian/cpp-socket-server)

[Demo](http://123.56.182.46:12345/index.html)

#### 功能

* [x] 单reactor多线程模式
* [x] 请求：GET、POST
* [x] 状态码（见[测试](#state)）
* [x] 较高并发（见[测试](#concurrency)）
* [x] 线程池任务分发
* [x] json格式解析（jsoncpp）
* [x] http报文解析（llhttp）
* [x] 缓存（redis）
* [x] url编解码
* [x] 滚动日志记录（spdlog）
* [x] 较高容错（异常信号捕获与处理）
* [x] HTTP报头处理（见[测试](#head)）

#### 环境

- ubuntu 20.04
- 安装g++：`sudo apt install -y g++`
- 安装jsoncpp：`sudo apt install -y libjsoncpp-dev`
- 安装redis：`sudo apt install -y redis-server && systemctl start redis`
- 安装hiredis：`sudo apt install -y libhiredis-dev`
- 安装spdlog：`sudo apt install -y libspdlog-dev`
- 安装llhttp：`git clone https://github.com/nodejs/llhttp.git && cd llhttp && sudo apt install -y npm && sudo npm install typescript && sudo apt install -y clang && sudo make && sudo make install && sudo cp build/libllhttp.so build/llhttp.h <makefile所在目录>`
- 添加共享库文件目录：`vim /etc/ld.so.conf`并添加一行`/usr/local/lib`然后刷新`/sbin/ldconfig -v`

#### 运行

1. 进入SERVER里, 清除生成的.o文件 : `make clean`
2. 执行make命令：`make`(debug：`make debug=true`, `gdb ./server`后run，或使用valgrind调试`valgrind ./server`)
3. 运行服务端: `./server [-p <端口号>] [-b <backlog>] [-c <true/false(是否使用缓存，默认不使用)>] [-t <true/false(是否使用多线程，默认使用)>]`,  默认端口12345，也可指定端口，如：./server -p 6666
4. 性能测试： 在`TEST/` 下有`multithreading/`、`origin/`、`select/`和`epoll/`四个文件夹分别对多线程、原始单路IO、select和epoll进行get性能测试，get的文件存放到`www/`里，编译运行后在其它窗口使用ab工具进行性能测试 (sudo apt-get install apache2-utils)

#### 最大线程数

1. 查看：编译：g++ test_max_thread.cpp -pthread test.o 运行：./test.o
2. 查看：cat /proc/sys/kernel/threads-max
3. 查看：sysctl kernel.threads-max
4. 修改：sudo sysctl -w kernel.threads-max=102400

#### 测试

- <span id="state">状态码</span>

|状态码|说明|
|:--|:--|
|200|OK|
|304|Not Modified|
|403|Forbidden|
|404|Not Found|
|500|Internal Server Error|

- <span id="head">报头</span>

|请求报头|响应报头|
|:--|:--|
|connection|connection|
|If-Modified-Since|Keep-Alive|
||Accept-Ranges|
||Content-Length|
||Last-Modified|
||Content-Type|

- UML图

![UML](./TEST/压测/UML.png)

- 模型测试

[ab test result.xlsx](./TEST/模型测试/ab_test_result.xlsx)

- 缓存测试（单线程430B）

    ![缓存测试](./TEST/压测/cache_test.png)

用Redis缓存文件虽然比通过read/write快，但远不如直接sendfile函数快，因为sendfile是零拷贝，已经够快了，而Redis还涉及到对象的创建销毁，字符串复制等，反而比sendfile慢了。

- <span id="concurrency">并发测试</span>
使用webbench测试（不使用缓存）：1000并发量测试时间60秒。与[Tkeed](https://github.com/linw7/TKeed)做对比

    ![并发测试](./TEST/压测/concurrency_test.png)

    多线程涉及线程阻塞等待等，并不一定有充分优势。2线程测试时性能下降，3线程时性能提升明显。并且sendfile()很快。

    测试最高并发10000

