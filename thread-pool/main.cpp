#include <iostream>
#include <functional>
#include <cmath>

#include "Task.h"
#include "ThreadPool.h"
#include "TestRunner.h"

void SingleTaskTest() {

    const int threads_num = 10;

    const int n = 20;
    const int fib = 6765;

    ThreadPool thread_pool(threads_num);

    auto func = [n]{
        int prev_fib = 1, curr_fib = 1;
        if (n == 1 ||  n == 2) return 1;
        for (auto _ = n - 2; _--;) {
            curr_fib += prev_fib;
            prev_fib =  curr_fib - prev_fib;
        }
        return curr_fib;
    };

    auto task = std::make_shared<Task<int>>(func);

    thread_pool.Enqueue(task);

    {
        AssertEqual(task->GetResult(), fib, "Single Task test ");
    }

    thread_pool.Dispose();
}

void MultipleTasksTest() {

    const int threads_num = 10;
    const int tasks_num = 20;

    const double true_amount = M_PI*2470;

    ThreadPool thread_pool(threads_num);

    std::vector<std::shared_ptr<Task<double>>> tasks;

    for (int i = 0; i < tasks_num; ++i) {
        double r = i;
        auto func = std::make_shared<Task<double>>([r]{
            return M_PI*r*r;
        });
        tasks.emplace_back(func);
        thread_pool.Enqueue(func);
    }

    double amount = 0;
    for(auto const& t: tasks) {
        amount += t->GetResult();
    }

    thread_pool.Dispose();

    {
        AssertEqual((int)amount, (int)true_amount, "Multiple tasks test");
    }
}

void ContinueWithTest() {
    const int threads_num = 10;

    const int n = 30;
    const int add_num = 10;
    const int prod_num = 10;

    const int fib_func_ans = 832040;
    const int add_func_ans = 832050;
    const int prod_func_ans = 8320500;

    ThreadPool thread_pool(threads_num);

    auto func = [n]{
        int prev_fib = 1, curr_fib = 1;
        if (n == 1 ||  n == 2) return 1;
        for (auto _ = n - 2; _--;) {
            curr_fib += prev_fib;
            prev_fib =  curr_fib - prev_fib;
        }
        return curr_fib;
    };

    auto add_func = [add_num](int sum) {
        return sum + add_num;
    };
    auto prod_func = [prod_num](int val) {
        return val * prod_num;
    };

    auto task = std::make_shared<Task<int>>(func);
    auto continue_task = task->ContinueWith<int>(add_func);
    auto second_continue_task = continue_task->ContinueWith<int>(prod_func);

    thread_pool.Enqueue(task);
    thread_pool.Enqueue(continue_task);
    thread_pool.Enqueue(second_continue_task);

    {
        AssertEqual(task->GetResult(), fib_func_ans, "first task in ContinueWithTest");
        AssertEqual(continue_task->GetResult(), add_func_ans, "continue task in ContinueWithTest");
        AssertEqual(second_continue_task->GetResult(), prod_func_ans, "second continue task in ContinueWithTest");
    }

    thread_pool.Dispose();
}

void ThreadsNumTest() {

    const int threads_num = 10;

    ThreadPool thread_pool(threads_num);

    {
        AssertEqual(thread_pool.NumOfThreads(), threads_num, "threads num test");
    }

    thread_pool.Dispose();
}

int main() {
    TestRunner tr;
    tr.RunTest(ThreadsNumTest, "ThreadsNumTest");
    tr.RunTest(SingleTaskTest, "SingleTaskTest");
    tr.RunTest(ContinueWithTest, "ContinueWithTest");
    tr.RunTest(MultipleTasksTest, "MultipleTasksTest");
}
