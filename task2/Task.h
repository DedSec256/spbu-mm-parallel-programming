//
// Created by garrancha on 06.05.2021.
//

#ifndef TASK2_TASK_H
#define TASK2_TASK_H

#include "Utils.h"


class ITask {


public:

    /**
     * @return whether or not this task is completed (either with success or with failure)
     */
    virtual bool IsCompleted() = 0;

    /**
     * @return whether or not this task is failed
     */
    virtual bool IsFailed() = 0;

    /**
     * Executes stored Job if it is not yet started.
     * Job computed only once.
     */
    virtual void Execute() = 0;


    virtual ~ITask() = default;;
};


template<class TResult>
class IMyTask : public ITask {

public:

    /**
     * Blocking call to obtain the result of this Task.
     * Can throw exception if task terminated with exception.
     * @return Result of computed Task
     */
    virtual TResult GetResult() = 0;

    /**
     * Provides access to underlying Task
     * @return underlying task
     */
    virtual std::shared_ptr<FuncWrapper<TResult>> GetTask() = 0;

};


template<class TResult>
class MyTask : public IMyTask<TResult> {

public:

    explicit MyTask(FuncWrapper<TResult> job) {
        func = std::make_shared<FuncWrapper<TResult>>(std::move(job));
    };

    MyTask() = delete;

    MyTask(const MyTask<TResult> &&other) = delete;

    std::shared_ptr<FuncWrapper<TResult>> GetTask() override { return func; };


    bool IsCompleted() override { return is_completed.load(std::memory_order_seq_cst); }

    bool IsFailed() override { return is_failed.load(std::memory_order_seq_cst); }


    void Execute() override {

        std::lock_guard<std::mutex> lock(mutex);

        // check if we have already computed this task
        if (is_completed.load(std::memory_order_seq_cst)) return;

        try {
            this->result = (*func)();
        } catch (std::exception &ex) {
            this->exception_msg = ex.what();
            this->is_failed.store(true, std::memory_order_seq_cst);
        }

        this->is_completed.store(true, std::memory_order_seq_cst);

        //can set that now Result can be obtained
        reset_event.Set();
    }

    TResult GetResult() override {

        reset_event.WaitOne();

        if (is_failed.load(std::memory_order_seq_cst)) throw TaskException(exception_msg);
        if (is_completed.load(std::memory_order_seq_cst)) return result;
    }


    template<class TNewResult>
    std::shared_ptr<MyTask<TNewResult>> ContinueWith(FuncWrapper<TNewResult, TResult> function) {
        auto f = [this, function] {
            TResult res = this->GetResult();
            return function(res);
        };

        return std::make_shared<MyTask<TNewResult>>(FuncWrapper<TNewResult>(std::move(f)));
    };

    ~MyTask() override = default;;

private:

    NoResetEvent reset_event;

    std::mutex mutex;

    std::atomic_bool is_completed{false};

    std::atomic_bool is_failed{false};

    std::shared_ptr<FuncWrapper<TResult>> func;

    TResult result;

    std::string exception_msg;

};

/**
 * Special case for a void type. We can not have void field as well as return void statement.
 * Difference in result field, ContinueWith and Execute methods
 */
template<>
class MyTask<void> : public IMyTask<void> {
public:

    explicit MyTask(FuncWrapper<void> job) {
        func = std::make_shared<FuncWrapper<void>>(std::move(job));
    };

    MyTask() = delete;

    MyTask(const MyTask<void> &&other) = delete;

    std::shared_ptr<FuncWrapper<void>> GetTask() override {
        return func;
    };


    bool IsCompleted() override { return is_completed.load(std::memory_order_seq_cst); }

    bool IsFailed() override { return is_failed.load(std::memory_order_seq_cst); }


    void Execute() override {

        std::lock_guard<std::mutex> lock(mutex);

        // check if we have already computed this task
        if (is_completed.load(std::memory_order_seq_cst)) return;

        try {
            (*func)();
        } catch (std::exception &ex) {
            this->exception_msg = ex.what();
            this->is_failed.store(true, std::memory_order_seq_cst);
        }

        this->is_completed.store(true, std::memory_order_seq_cst);

        //can set that now Result can be obtained
        reset_event.Set();
    }

    void GetResult() override {

        reset_event.WaitOne();

        if (is_failed.load(std::memory_order_seq_cst)) throw TaskException(exception_msg);
        if (is_completed.load(std::memory_order_seq_cst)) return;
    }


    template<class TNewResult>
    std::shared_ptr<MyTask<TNewResult>> ContinueWith(FuncWrapper<TNewResult, void> function) {
        auto f = [this, function] {
            this->GetResult();
            return function();
        };

        return std::make_shared<MyTask<TNewResult>>(FuncWrapper<TNewResult>(std::move(f)));
    };

    ~MyTask() override = default;

private:

    NoResetEvent reset_event;

    std::mutex mutex;

    std::atomic_bool is_completed{false};

    std::atomic_bool is_failed{false};

    std::shared_ptr<FuncWrapper<void>> func;

    std::string exception_msg;

};


#endif //TASK2_TASK_H
