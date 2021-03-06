#include "thread_pool.hpp"

ThreadPool::ThreadPool(size_t size) 
    : local_task_queues(size)
{
    for (size_t i = 0; i < size; ++i)
        threads.emplace_back(*this, i);
}

void ThreadPool::Enqueue(Func && task, size_t thread_queue_id) {
    if (thread_queue_id == -1)
        task_queue.Push(move(task));
    else
        local_task_queues[thread_queue_id].Push(move(task));
}

ThreadPool::SThread::SThread(ThreadPool & pool, size_t id)
    : id(id)
    , pool(pool)
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

        pool.RunPendingTask(id);
    }
}

void ThreadPool::RunPendingTask(size_t id) noexcept {
    Func foo;
    if (!local_task_queues[id].TryPop(foo) &&
        !task_queue.TryPop(foo) && 
        !TrySteal(foo, id))
    {
        // Just let other threads/processes do work.
        // Rewrite this line with conditional_variables if threads should sleep when task_queue is empty.
        std::this_thread::yield();
    } else {
        (*foo)();
        for (auto & conts : foo->continuations)
            Enqueue(move(conts), id);
    }
}

bool ThreadPool::TrySteal(Func & res, size_t current_id) noexcept {
    const auto n = local_task_queues.size();
    for (size_t i = 1; i < n; ++i) {
        if (local_task_queues[(current_id + i) % n].TryPop(res))
            return true;
    }
    return false;
}
