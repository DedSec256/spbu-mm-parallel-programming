#pragma once
#include "task.hpp"
#include "thread_safe_queue.hpp"
#include <thread>
#include <atomic>
#include <list>
#include <condition_variable>

class ThreadPool {
    using Func = std::unique_ptr<Callable>;
    class SThread {
        ThreadPool & pool;
        std::atomic<bool> stop;
        std::thread thread;

    public:
        SThread(ThreadPool & pool);
        ~SThread();
        void operator()() noexcept;
    };
    friend class SThread;

    std::list<SThread> threads;
    TSQueue<Func> task_queue;
public:
    ThreadPool(size_t size);

    void Enqueue(Func && task);
    void RunPendingTask() noexcept;
};
