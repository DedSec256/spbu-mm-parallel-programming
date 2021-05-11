//
// Created by garrancha on 02.05.2021.
//

#ifndef TASK2_THREADPOOL_H
#define TASK2_THREADPOOL_H

#include "ThreadSafeDeque.h"
#include "Task.h"

class MyThreadPool {

    typedef std::shared_ptr<ITask> Task;
    typedef ThreadSafeDeque<Task> TaskDeque;
public:

    explicit MyThreadPool(int num_threads) {

        distribution = std::uniform_int_distribution<int>(0, num_threads - 1);
        threads = std::vector<std::thread>();


        for (int i = 0; i < num_threads; ++i)
            thread_deques.push_back(std::move(std::make_shared<TaskDeque>()));

        std::atomic_thread_fence(std::memory_order_seq_cst);

        for (int i = 0; i < num_threads; ++i)
            threads.push_back(std::move(std::thread(&MyThreadPool::RunWorker, this, i)));


    }

    template<class TResult>
    bool Enqueue(std::shared_ptr<IMyTask<TResult>> task) {

        auto idx = distribution(generator);

        return thread_deques[idx]->push_front(std::move(task));
    }

    bool IsAllThreadAlive() {
        return std::all_of(threads.begin(), threads.end(), [](auto &&x) { return x.joinable(); });
    };

    void Dispose() {
        for (auto &q : thread_deques) q->block();
        is_running.store(false, std::memory_order_seq_cst);
    }



    ~MyThreadPool() {
        Dispose();
        for (auto &thread : threads) thread.join();
    }

private:


    bool stealTask(int idx, Task &task) {

        for (int i = 1; i < threads.size(); ++i) {
            auto thread_idx_to_steal = (idx + i) % threads.size();
            auto thread_local_queues = thread_deques[thread_idx_to_steal];
            if (thread_local_queues->pop_front(task)) return true;
        }

        return false;
    }


    void RunWorker(int idx) {

        auto thread_local_queue = thread_deques[idx];

        bool should_finish = false;

        while (true) {
            try {
                Task task;

                // if we no longer accept new tasks
                if (!should_finish && !is_running.load(std::memory_order_seq_cst)) {
                    should_finish = true;
                }


                if (thread_local_queue->pop_back(task)) {
                    task->Execute();
                } else if (stealTask(idx, task)) {
                    task->Execute();
                } else {

                    // thread cannot obtain any task from any queues -> no work, can terminate
                    if (task == nullptr && should_finish) break;
                    std::this_thread::yield();
                }


            } catch (std::exception &ex) {
                std::cout << "Some troubles with inner logic or system exception" << std::endl;
                std::cout << ex.what() << std::endl;
                Dispose();
            }
        }
    }



    std::atomic<bool> is_running{true};
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;
    std::vector<std::thread> threads;
    std::vector<std::shared_ptr<TaskDeque>> thread_deques;

};


#endif //TASK2_THREADPOOL_H
