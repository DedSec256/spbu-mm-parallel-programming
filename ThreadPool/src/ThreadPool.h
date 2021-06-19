//
// Created by 19216 on 2021/5/8.
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "IMyTask.h"

using namespace std;

class IDisposable{
public:
    // 添加任务(Добавить задачу)
    virtual void Enqueue(IMyTask *task) = 0;
    // 释放资源(Освободить ресурсы)
    virtual void Dispose() = 0;
protected:
    virtual ~IDisposable() {}
};

class ThreadPool : public IDisposable{
private:

    vector<thread> threadPool;  // 线程池(Пул потоков)
    queue<IMyTask*> taskQueue;      // 任务队列(Очередь задач)
    int threadNum;              // 线程池中线程数量(Количество потоков в пуле потоков)

    // 同步(Синхронизировать)
    mutex mutexTask;
    condition_variable conditionVariableTask;
    mutex mutexIO;

    atomic<bool> stop;

public:

    explicit ThreadPool(int number);
    ~ThreadPool();

    // 添加任务(Добавить задачу)
    void Enqueue(IMyTask *task) override;
    // 结束(конец)
    void Dispose() override;

private:

    // 开启线程的运行(Начать выполнение потока)
    void start();
    // 获取一个任务(Получить задание)
    IMyTask* getOneTask();

};


#endif //THREADPOOL_THREADPOOL_H
