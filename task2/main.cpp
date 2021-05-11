#include <iostream>

#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include <atomic>
#include <utility>
#include <condition_variable>
#include <random>

#include "Task.h"
#include "ThreadPool.h"



std::unique_ptr<MyThreadPool> create_threadpool(int num_threads) {
    return std::make_unique<MyThreadPool>(num_threads);
}


/**
 * Test that checks addition of one task to thread pool
 * @param num_threads
 */
bool add_one_task_test(int num_threads) {

    std::atomic<bool> result{false};

    {
        auto thread_pool = create_threadpool(num_threads);
        auto funcw = FuncWrapper<void>([&result] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "I am completed" << std::endl << std::flush;
            result.store(true, std::memory_order_seq_cst);
        });
        auto mytask = std::make_shared<MyTask<void>>(funcw);

        thread_pool->Enqueue<void>(mytask);

        thread_pool->Dispose();
    }

    return result.load(std::memory_order_seq_cst);
}

/**
 * Test that checks correctness of ContinueWith method
 * @param num_threads
 */
bool continue_with_test(int num_threads) {

    int result;
    int init = 19;
    int conin = 23;
    {
        auto thread_pool = create_threadpool(num_threads);
        auto funcw1 = FuncWrapper<int>([init] {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            return init;
        });

        auto funcw2 = FuncWrapper<int, int>([conin](int a) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            std::cout << "I am continuation" << std::endl << std::flush;
            return conin + a;
        });
        auto mytask = std::make_shared<MyTask<int>>(funcw1);
        auto mytask2 = mytask->ContinueWith(funcw2);


        thread_pool->Enqueue<int>(mytask);
        thread_pool->Enqueue<int>(mytask2);

        auto funcw3 = FuncWrapper<void>([]() {
            std::cout << "I am void guy" << std::endl << std::flush;
        });
        thread_pool->Enqueue<void>(std::make_shared<MyTask<void>>(funcw3));

        result = mytask2->GetResult();
    }

    return result == (conin + init);
}


/**
 * Test that checks that task throw exception when computation terminate with some exception
 * @param num_threads
 */
bool continue_with_exception_test(int num_threads) {

    {
        auto thread_pool = create_threadpool(num_threads);
        auto funcw1 = FuncWrapper<int>([] {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            throw std::runtime_error("error 42");
            return 5;
        });

        auto funcw2 = FuncWrapper<int, int>([](int a) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            std::cout << "I am continuation that should not appeared" << std::endl << std::flush;
            return 0;
        });
        auto mytask = std::make_shared<MyTask<int>>(funcw1);
        auto mytask2 = mytask->ContinueWith(funcw2);

        thread_pool->Enqueue<int>(mytask);
        thread_pool->Enqueue<int>(mytask2);

        auto funcw3 = FuncWrapper<void>([]() {
            std::cout << "I am void guy" << std::endl << std::flush;
        });
        thread_pool->Enqueue<void>(std::make_shared<MyTask<void>>(funcw3));

        std::string msg;
        try {
            mytask2->GetResult();
        } catch (TaskException &ex) {
            msg = ex.what();
            std::cout << "Handled custrom task exception:" << std::endl << std::flush;
            std::cout << msg << std::endl << std::flush;
        }

        return mytask2->IsFailed();

    }
}

/**
 * Test that checks that task throw exception when computation terminate with some exception
 * @param num_threads
 */
bool all_threads_alive_test(int num_threads) {
    bool is_alive;
    {
        auto thread_pool = create_threadpool(num_threads);
        auto funcw1 = FuncWrapper<int>([] {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            throw std::runtime_error("error 42");
            return 5;
        });

        auto mytask1 = std::make_shared<MyTask<int>>(funcw1);
        auto mytask2 = std::make_shared<MyTask<int>>(funcw1);

        thread_pool->Enqueue<int>(mytask1);
        thread_pool->Enqueue<int>(mytask2);

        auto funcw3 = FuncWrapper<void>([]() {
            std::cout << "I am void guy" << std::endl << std::flush;
        });
        thread_pool->Enqueue<void>(std::make_shared<MyTask<void>>(funcw3));


        is_alive = thread_pool->IsAllThreadAlive();
        thread_pool->Dispose();
    }

    return is_alive;
}


/**
 * Test that checks that task throw exception when computation terminate with some exception
 * @param num_threads
 */
bool add_more_task_then_threads(int num_threads) {
    std::atomic<int> completed_tasks{0};

    int num_tasks = 5 * num_threads;
    {

        auto thread_pool = create_threadpool(num_threads);


        for (int i = 0; i < num_tasks; ++i) {
            auto funcw1 = FuncWrapper<int>([&completed_tasks] {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                completed_tasks++;
                return 42;
            });
            auto mytask1 = std::make_shared<MyTask<int>>(funcw1);

            auto funcw2 = FuncWrapper<int, int>([&completed_tasks](int a) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                completed_tasks++;
                return a;
            });
            auto continuation = mytask1->ContinueWith(funcw2);

            thread_pool->Enqueue<int>(mytask1);
            thread_pool->Enqueue<int>(continuation);
        }

        thread_pool->Dispose();
    }
    // twice since we continue with
    return 2 * num_tasks == completed_tasks;
}


int main() {
    bool result;

    std::cout << "-------------------------" << std::endl << std::flush;
    std::cout << "Test with one task addition..." << std::endl << std::flush;
    result = add_one_task_test(4);
    std::cout << "Test status: " << (result ? "Completed" : "Failed") << std::endl << std::flush;
    std::cout << "-------------------------";
    std::cout << std::endl << std::flush;

    std::cout << "Test with continue with..." << std::endl << std::flush;
    result = continue_with_test(4);
    std::cout << "Test status: " << (result ? "Completed" : "Failed") << std::endl << std::flush;
    std::cout << "-------------------------";
    std::cout << std::endl << std::flush;


    std::cout << "Test with continue with when task yield exception..." << std::endl << std::flush;
    result = continue_with_exception_test(4);
    std::cout << "Test status: " << (result ? "Completed" : "Failed") << std::endl << std::flush;
    std::cout << "-------------------------" << std::endl << std::flush;

    std::cout << "Test to check that all threads alive..." << std::endl << std::flush;
    result = all_threads_alive_test(4);
    std::cout << "Test status: " << (result ? "Completed" : "Failed") << std::endl << std::flush;
    std::cout << "-------------------------" << std::endl << std::flush;


    std::cout << "Test to check correctness when tasks more than working threads..." << std::endl << std::flush;
    result = add_more_task_then_threads(4);
    std::cout << "Test status: " << (result ? "Completed" : "Failed") << std::endl << std::flush;
    std::cout << "-------------------------" << std::endl << std::flush;

    return 0;
}
