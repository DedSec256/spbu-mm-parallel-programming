//
// Created by Мунира on 08.05.2021.
//

#include "Buffer.h"
#include <thread>
#include <atomic>

#ifndef PRODUCER_CONSUMER_WORKER_H
#define PRODUCER_CONSUMER_WORKER_H

template<typename T>
class Worker {
public:
    explicit Worker(Buffer<T> &new_buffer, T& productivity): buffer(new_buffer), isStopped(false), productivity(productivity) {
        this->thread = std::thread(&Worker::run, this);
    };
    void stop(){
        isStopped = true;
    };
    virtual void run() = 0;
protected:
    Buffer<T>& buffer;
    std::thread thread;
    std::atomic<bool> isStopped;
    void sleep(){
        std::this_thread::sleep_for (std::chrono::seconds(3));
    };
    T& productivity;
};

#endif //PRODUCER_CONSUMER_WORKER_H
