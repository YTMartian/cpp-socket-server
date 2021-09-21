CPP socket编程实现web后台服务器。

* [x] 支持GET请求
* [x] 支持POST请求
* [ ] 支持HEAD请求
* [x] 支持状态码： 404、500
* [ ] 日志记录
* [x] 高并发，最大支持


#### 运行

1. 进入multithreading/、origin/、select/或epoll/里, 清除生成的.o文件 : make clean
2. 执行make命令：make
3. 运行服务端: ./server 默认端口8000，也可指定端口，如：./server 1234
4. 在浏览器中打开，如：127.0.0.1:8000/login.html  若文件不存在会返回404页面
5. 性能测试： 使用ab工具

#### 最大线程数

1. 查看：编译：g++ test_max_thread.cpp -pthread test.o 运行：./test.o
2. 查看：cat /proc/sys/kernel/threads-max
3. 查看：sysctl kernel.threads-max
4. 修改：sudo sysctl -w kernel.threads-max=102400

#### 其它

1. 使用[jsoncpp](https://github.com/open-source-parsers/jsoncpp)解析json数据。
2. 使用sqlite3数据库进行数据存储。
3. 使用redis缓存用户名、密码。
4. 后期改进参考[https://sqlite.org/althttpd/doc/trunk/althttpd.md](https://sqlite.org/althttpd/doc/trunk/althttpd.md)，一个sqlite作者实现的简单http服务器（只有单个文件），支持GET、POST、HEAD请求。
