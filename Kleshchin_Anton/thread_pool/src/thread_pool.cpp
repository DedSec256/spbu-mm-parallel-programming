#include "thread_pool.hpp"

ThreadPool::ThreadPool(size_t size) {
    for (size_t i = 0; i < size; ++i)
        threads.emplace_back(*this);
}

void ThreadPool::Enqueue(Func && task) {
    task_queue.Push(std::move(task));
}

ThreadPool::SThread::SThread(ThreadPool & pool)
    : pool(pool)
    , stop(false)
    , thread(std::ref(*this))
{}

ThreadPool::SThread::~SThread() {
    stop = true;
    thread.join();
}

void ThreadPool::SThread::operator()() noexcept {
    while (true) {
        if (stop)
            return;

        pool.RunPendingTask();
    }
}

void ThreadPool::RunPendingTask() noexcept {
    Func foo;
    if (!task_queue.TryPop(foo)) {
        // Just let other threads/processes do work.
        // Rewrite this line with conditional_variables if threads should sleep when task_queue is empty.
        std::this_thread::yield();
    } else {
        (*foo)();
        for (auto & conts : foo->continuations)
            Enqueue(move(conts));
    }
}
