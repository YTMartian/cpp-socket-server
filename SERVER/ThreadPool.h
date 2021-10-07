//
// Created by tim on 2021/10/6.
//

#ifndef SERVER_THREADPOOL_H
#define SERVER_THREADPOOL_H

#include "head.h"
#include "Processor.h"

//https://ncona.com/2019/05/using-thread-pools-in-cpp/
class ThreadPool {
private:
    //这个条件变量告诉线程池等待，直到有任务出现
    condition_variable_any workQueueConditionVariable;
    //保存线程
    vector<thread> threads;
    //互斥锁保护工作队列
    mutex workQueueMutex;
    //等待进行处理的任务队列
    queue<shared_ptr<Processor>> workQueue;
    //线程池是否关机
    bool shutdown;

    //处理任务队列
    void doWork();

    //处理请求
    static void processRequest(const shared_ptr<Processor>&);

public:

    ThreadPool();

    //join，等待所有线程结束
    ~ThreadPool();

    //添加一个任务
    void addWork(const shared_ptr<Processor>&);
};


#endif //SERVER_THREADPOOL_H
