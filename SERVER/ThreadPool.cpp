//
// Created by tim on 2021/10/6.
//

#include "ThreadPool.h"

ThreadPool::ThreadPool() {
    shutdown = false;
    //获取硬件支持的并发数（CPU核心数），如果无法获取则是0，就默认为单线程
    auto numberOfThreads = thread::hardware_concurrency();
    if (numberOfThreads == 0) {
        numberOfThreads = 1;
    }

    for (unsigned i = 0; i < numberOfThreads; ++i) {
        //创建numberOfThreads个线程，传入函数和参数
        threads.emplace_back(thread(&ThreadPool::doWork, this));
    }
}

ThreadPool::~ThreadPool() {
    //当前线程池已经关机
    shutdown = true;

    //唤醒所有线程，以让它们能够join
    workQueueConditionVariable.notify_all();
    for (auto &thread: threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void ThreadPool::addWork(const shared_ptr<Processor> &processor) {
    //自动释放锁，其原理是：声明一个局部的lock_guard对象，在其构造函数中进行加锁，在其析构函数中进行解锁。最终的结果就是：在定义该局部对象的时候加锁（调用构造函数），出了该对象作用域的时候解锁（调用析构函数）。
    lock_guard<mutex> g(workQueueMutex);
    //将请求任务添加进队列之中
    workQueue.push(processor);

    //通知一个线程处理任务
    workQueueConditionVariable.notify_one();
}

void ThreadPool::doWork() {
    while (!shutdown) {
        shared_ptr<Processor> processor;
        //为自动释放锁设定一个作用域
        {
            unique_lock<mutex> g(workQueueMutex);
            workQueueConditionVariable.wait(g, [&] {
                //当任务队列中有任务或关机时才唤醒
                return !workQueue.empty() || shutdown;
            });
            //获取队列最前面的任务
            processor = workQueue.front();
            workQueue.pop();
        }
        processRequest(processor);
    }
}

void ThreadPool::processRequest(const shared_ptr<Processor>& processor) {
    try {
        processor->run();
    } catch (exception &e) {
        logger->error("请求执行失败: {}({})", __FILE__, __LINE__);
    }
    auto client_sockfd = processor->get_client_sockfd();
    auto keep_alive = processor->get_keep_alive();
    //多线程时只能关闭，否则write时会产生Broken pipe（errno=32）的问题,
    if(!keep_alive) close(client_sockfd);
}