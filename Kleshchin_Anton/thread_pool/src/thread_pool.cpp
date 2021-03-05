#include "thread_pool.hpp"

ThreadPool::ThreadPool(size_t size) {
    for (size_t i = 0; i < size; ++i)
        threads.emplace_back(*this);
}

void ThreadPool::Enqueue(Func && task) {
    task_queue.Put(std::move(task));
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

        Func foo;
        if (!pool.task_queue.Take(foo)) {
            std::this_thread::yield();
        } else {
            (*foo)();
            for (auto & conts : foo->continuations)
                pool.task_queue.Put(move(conts));
        }
    }
    
}
