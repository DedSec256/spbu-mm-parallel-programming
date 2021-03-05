#pragma once
#include "thread_safe_storage.hpp"
#include <functional>
#include <atomic>
#include <thread>
#include <chrono>
#include <random>

template<class T>
class Worker {
    std::mt19937 random_generator;
protected:
    TSQueue<T> & storage;
    std::atomic<bool> stop;

    void RandomSleep() noexcept {
        auto sleep_time = (random_generator() & 0xff) + 1;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
public:
    Worker(TSQueue<T> & storage);
    void Stop() noexcept { stop = true; }
};

template<class T>
class Producer : public Worker<T> {
    std::function<T()> producer;
public:
    Producer(TSQueue<T> & storage, std::function<T()> producer);
    void operator()() noexcept;
};

template<class T>
class Consumer : public Worker<T> {
    std::function<void(T&&)> consumer;
public:
    Consumer(TSQueue<T> & storage, std::function<void(T&&)> consumer);
    void operator()() noexcept;
};

template<class T>
class WorkerThread {
    std::reference_wrapper<T> rw;
    std::thread thread;
public:
    WorkerThread(std::reference_wrapper<T> rw)
        : rw(rw)
        , thread(rw)
    {}

    ~WorkerThread() {
        rw.get().Stop();
        thread.join();
    }
};

template<class T>
Worker<T>::Worker(TSQueue<T> & storage)
    : random_generator(std::random_device()())
    , storage(storage)
    , stop(false)
{}

template<class T>
Producer<T>::Producer(TSQueue<T> & storage, std::function<T()> producer)
    : Worker<T>(storage)
    , producer(std::move(producer))
{}

template<class T>
Consumer<T>::Consumer(TSQueue<T> & storage, std::function<void(T&&)> consumer)
    : Worker<T>(storage)
    , consumer(std::move(consumer))
{}

template<class T>
void Producer<T>::operator()() noexcept {
    while (!this->stop) {
        this->storage.Put(producer());
        this->RandomSleep();
    }
}

template<class T>
void Consumer<T>::operator()() noexcept {
    while (!this->stop) {
        T value;
        if (this->storage.Take(value))
            consumer(std::move(value));
        this->RandomSleep();
    }
}
