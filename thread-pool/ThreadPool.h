#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>

#include "Task.h"

#ifndef THREAD_POOL_THREADPOOL_H
#define THREAD_POOL_THREADPOOL_H


template <typename T>
struct atomic_wrapper
{
    std::atomic<T> _a;

    atomic_wrapper() :_a() {}

    explicit atomic_wrapper(const std::atomic<T> &a) :_a(a.load()) {}

    atomic_wrapper(const atomic_wrapper &other) :_a(other._a.load()) {}

    atomic_wrapper &operator=(const atomic_wrapper &other) {
        _a.store(other._a.load());
    }
};

class ThreadPool {
public:

    explicit ThreadPool(int threadsNum) : is_running(true) {
        for (int i = 0; i < threadsNum; ++i) {
            thread_queue.emplace_back(std::queue<std::shared_ptr<ITask>>());

            std::atomic<bool> a_i(false);
            is_thread_stopped.emplace_back(a_i);
        }
        for (int i = 0; i < threadsNum; i++) {
            log("started thread");
            threads.emplace_back(std::thread(&ThreadPool::Run, this, i));
        }
    }

    void Enqueue(std::shared_ptr<ITask> task) {
        int i = rand()%threads.size();

        std::scoped_lock<std::mutex> lock(mutex);
        if (!is_thread_stopped[i]._a.load()) {
            thread_queue[i].push(task);
        }
        log("pushed task in thread");
    }

    void Dispose() {
        for (int i = 0; i < threads.size(); i++) {
            std::scoped_lock<std::mutex> lock(mutex);
            is_thread_stopped[i]._a.store(true);
        }
        is_running.store(false);
        Join();
    }

    void Join() {
        for (auto& t : threads)
            if (t.joinable())
                t.join();
    }

    int NumOfThreads() {
        int cnt = 0;
        for (auto& t : threads) {
            if (t.joinable())
                cnt++;
        }
        return cnt;
    }

    ~ThreadPool() {
        Dispose();
        Join();
    }

private:
    void Run(int idx) {
        while (is_running) {
            std::shared_ptr<ITask> task;

            std::scoped_lock<std::mutex> lock(mutex);
            if (!thread_queue[idx].empty()) {
                task = std::move(thread_queue[idx].front());
                thread_queue[idx].pop();
                mutex.unlock();
                try {
                    task->Run();
                } catch (const std::exception& e) {
                    std::throw_with_nested(e);
                }
                log("found task from local queue");
                continue;
            }
            mutex.unlock();

            bool task_found = false;

            for (int i = 1; i < threads.size(); ++i) {
                if (i != idx) {
                    std::scoped_lock<std::mutex> lock(mutex);
                    if (!thread_queue[i].empty()) {
                        task = std::move(thread_queue[i].front());
                        thread_queue[i].pop();
                        try {
                            task->Run();
                        } catch (const std::exception& e) {
                            std::throw_with_nested(e);
                        }
                        task_found = true;
                        log("found task from different threads queue");
                        mutex.unlock();
                        break;
                    }
                    mutex.unlock();
                }
            }

            if (!task_found){
                log("no tasks found");
                std::this_thread::yield();
            }
        }
        log("finished thread");
    }

    void log(std::string text) {
        std::scoped_lock<std::mutex> locker(log_mutex);
        std::cout << "Thread: " << std::this_thread::get_id() << ". Action: " << text << std::endl;
    }

    std::atomic<bool> is_running;
    std::vector<std::thread> threads;
    std::mutex mutex;
    std::mutex log_mutex;
    std::vector<std::queue<std::shared_ptr<ITask>>> thread_queue;
    std::vector<atomic_wrapper<bool>> is_thread_stopped;
};

#endif //THREAD_POOL_THREADPOOL_H
