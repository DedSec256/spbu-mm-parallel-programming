#include <iostream>
#include <functional>
#include <atomic>
#include <thread>

#ifndef THREAD_POOL_TASK_H
#define THREAD_POOL_TASK_H

class ITask {
public:
    virtual ~ITask() = default;
    virtual void Run() = 0;
    virtual bool IsCompleted() = 0;
};

template <typename R, typename ...A>
class Task : public virtual ITask {
public:
    explicit Task(std::function<R(A...)> func) : func(func), is_completed(false) {}

    R GetResult() {
        while (!is_completed.load()) {
            std::this_thread::yield();
        }
        return result;
    }

    void Run() override {
        result = func();
        is_completed.store(true);
    }

    bool IsCompleted() override {
        return is_completed.load();
    }

    template <typename NR>
    std::shared_ptr<Task<NR>> ContinueWith(std::function<NR(R)> continueFunc) {
        auto f = [this, continueFunc] {
            R res = this->GetResult();
            return continueFunc(res);
        };
        return std::make_shared<Task<NR>>(f);
    }

private:
    std::atomic<bool> is_completed;
    std::function<R(A...)> func;
    R result;
};


#endif //THREAD_POOL_TASK_H
