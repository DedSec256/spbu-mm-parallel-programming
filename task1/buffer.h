//
// Created by garrancha on 26.04.2021.
//


#ifndef TASK1_BUFFER_H
#define TASK1_BUFFER_H


#include <atomic>

template<class Task>
class Buffer {
public:

    /**
     * Add task to buffer
     */
    virtual void Push(Task task) = 0;

    /**
     * Pop task from Buffer.
     * If the operation succeeded then the result will be stored in pop_task and the method returns True
     */
    virtual bool Pop(Task &pop_task) = 0;

    virtual ~Buffer() = default;

};


/**
 * a thread safe buffer that uses deque and force threads to sleep for  WAIT_MS if they perform 2 subsequent access on pop or on push
 * @tparam Task
 * @tparam WAIT_MS
 */
template<class Task, int WAIT_MS>
class FairDequeBuffer : public Buffer<Task> {

public:

    explicit FairDequeBuffer() : Buffer<Task>() {};

    FairDequeBuffer(const FairDequeBuffer<Task, WAIT_MS> &other) = delete;

    FairDequeBuffer &operator=(const FairDequeBuffer<Task, WAIT_MS> &) = delete;

    void Push(Task task) override {
        std::unique_lock<std::mutex> guard(m, std::defer_lock);

        auto cur_thread_id = std::this_thread::get_id();

        guard.lock();

        deq.push_front(std::move(task));

        auto should_wait = cur_thread_id == last_push_thread.load(std::memory_order_seq_cst);

        last_push_thread.store(cur_thread_id,std::memory_order_seq_cst);

        guard.unlock();


        if (should_wait) {
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_MS));
        }

    }


    bool Pop(Task &pop_task) override {
        std::unique_lock<std::mutex> guard(m, std::defer_lock);

        auto cur_thread_id = std::this_thread::get_id();
        auto should_wait = false;

        auto is_successful_pop = false;

        guard.lock();

        if (!deq.empty()) {
            pop_task = std::move(deq.back());
            deq.pop_back();

            should_wait = cur_thread_id == last_pop_thread.load(std::memory_order_seq_cst);

            last_pop_thread.store(cur_thread_id, std::memory_order_seq_cst);
            is_successful_pop = true;
        };

        guard.unlock();


        if (should_wait) {
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_MS));
        }

        return is_successful_pop;
    }

    int size() {
        std::lock_guard<std::mutex> lock(m);
        return deq.size();
    }


private:
    std::atomic<std::thread::id> last_push_thread{std::thread::id()};
    std::atomic<std::thread::id> last_pop_thread{std::thread::id()};

    std::mutex m;
    std::deque<Task> deq;
};

#endif //TASK1_BUFFER_H
