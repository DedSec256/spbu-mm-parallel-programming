//
// Created by Мунира on 07.05.2021.
//

#include <queue>
#include <mutex>
#include <optional>

#ifndef PRODUCER_CONSUMER_BUFFER_H
#define PRODUCER_CONSUMER_BUFFER_H

template<typename T>
class Buffer {
public:
    std::optional<T> pop(){
        std::scoped_lock<std::mutex> lock(mutex);
        if(storage.empty()){
            return {};
        }
        T value = storage.front();
        storage.pop();
        mutex.unlock();
        return value;
    };
    void push(T &value){
        std::scoped_lock<std::mutex> lock(mutex);
        storage.push(value);
        mutex.unlock();
    };
    bool empty(){
        std::scoped_lock<std::mutex> lock(mutex);
        bool res = storage.empty();
        mutex.unlock();
        return res;
    };
    int size(){
        std::scoped_lock<std::mutex> lock(mutex);
        return storage.size();
    }
private:
    std::queue<T> storage;
    std::mutex mutex;
};


#endif //PRODUCER_CONSUMER_BUFFER_H
