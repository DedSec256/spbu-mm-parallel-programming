#pragma once
#include "task.hpp"
#include "thread_safe_queue.hpp"
#include <thread>
#include <atomic>
#include <list>
#include <condition_variable>

class ThreadPool {
    using Func = std::unique_ptr<Callable>;
    using TaskQueue = TSQueue<Func>;

    class SThread {
        size_t id;
        ThreadPool & pool;
        std::atomic<bool> stop;
        std::thread thread;
    public:
        SThread(ThreadPool & pool, size_t id);
        ~SThread();
        void operator()() noexcept;
    };
    friend class SThread;

    std::list<SThread> threads;
    TaskQueue task_queue;
    std::vector<TaskQueue> local_task_queues;
public:
    ThreadPool(size_t size);

    void Enqueue(Func && task, size_t thread_queue_id = -1);
    void RunPendingTask(size_t id = 0) noexcept;

private:
    bool TrySteal(Func & res, size_t current_id) noexcept;
};
