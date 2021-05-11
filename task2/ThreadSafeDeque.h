//
// Created by garrancha on 06.05.2021.
//

#ifndef TASK2_THREADSAFEDEQUE_H
#define TASK2_THREADSAFEDEQUE_H

template<class T>
class ThreadSafeDeque {

public:
    explicit ThreadSafeDeque() : deque(std::deque<T>()) {};

    ThreadSafeDeque(const ThreadSafeDeque<T> &other) = delete;

    ThreadSafeDeque &operator=(const ThreadSafeDeque<T> &other) = delete;

    bool push_front(T value) {
        std::lock_guard<std::mutex> lk(m);
        if (!stop.load(std::memory_order_seq_cst)) {
            deque.push_front(std::move(value));
            return true;
        }

        return false;
    }

    bool pop_front(T &result) {
        std::lock_guard<std::mutex> lk(m);
        if (deque.empty()) return false;
        result = std::move(deque.front());
        deque.pop_front();
        return true;
    }

    bool pop_back(T &result) {
        std::lock_guard<std::mutex> lk(m);
        if (deque.empty()) return false;
        result = std::move(deque.back());
        deque.pop_back();
        return true;
    }

    void block() {
        std::lock_guard<std::mutex> lk(m);
        stop.store(true,std::memory_order_seq_cst);
    }

private:
    std::mutex m;
    std::deque<T> deque;
    std::atomic_bool stop{false};
};


#endif //TASK2_THREADSAFEDEQUE_H
