//
// Created by garrancha on 26.04.2021.
//

#ifndef TASK1_WORKERS_H
#define TASK1_WORKERS_H

#include <atomic>

/**
 *
 * @tparam Task
 */
template<class Task>
class Worker {

public:
    explicit Worker(std::shared_ptr<Buffer<Task>> task_buffer) : task_buff(std::move(task_buffer)),
                                                                 cancel_production(false),
                                                                 is_finished(false) {};


    void cancel() { cancel_production.store(true, std::memory_order_seq_cst); }

    bool is_worker_finish() { return is_finished.load(std::memory_order_seq_cst); }

    void join() {
        std::lock_guard<std::mutex> lk(m_join);
        cancel();
        if (!is_joined.load(std::memory_order_seq_cst)) {
            is_joined.store(true, std::memory_order_seq_cst);
            this->worker.join();
        }
    }

    virtual ~Worker() = default;

protected:
    std::shared_ptr<Buffer<Task>> task_buff;
    std::thread worker;

    virtual void WorkerJob() = 0;

    void run() {

        this->worker = std::thread([this] {

            while (!cancel_production.load(std::memory_order_seq_cst))
                WorkerJob();

            is_finished.store(true, std::memory_order_seq_cst);
        });
    }

private:
    std::atomic<bool> cancel_production{};

    std::atomic<bool> is_joined{false};
    std::mutex m_join;

    std::atomic<bool> is_finished{false};
};


template<class Task>
class ProducerWorker : public Worker<Task> {


public:

    explicit ProducerWorker(std::shared_ptr<Buffer<Task>> buffer, std::function<Task()> task_provider) :
            Worker<Task>(std::move(buffer)), task_provider(task_provider) {
        this->run();
    }

    ~ProducerWorker() {
        this->join();
    }

protected:

    void WorkerJob() override {
        auto task = task_provider();
        this->task_buff->Push(task);
    }

private:
    std::function<Task()> task_provider;
};


template<class Task>
class ConsumerWorker : public Worker<Task> {

public:
    explicit ConsumerWorker(std::shared_ptr<Buffer<Task>> buffer) : Worker<Task>(std::move(buffer)) {
        this->run();
    }

    ~ConsumerWorker() {
        this->join();
    }

protected:
    void WorkerJob() override {
        Task task;
        auto is_successful = this->task_buff->Pop(task);
        if (is_successful) task();
    }


};


#endif //TASK1_WORKERS_H
