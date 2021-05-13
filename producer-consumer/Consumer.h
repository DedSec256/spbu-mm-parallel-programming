//
// Created by Мунира on 08.05.2021.
//

#include "Worker.h"
#include "Buffer.h"
#include <optional>

#ifndef PRODUCER_CONSUMER_CONSUMER_H
#define PRODUCER_CONSUMER_CONSUMER_H

template<typename T>
class Consumer : public Worker<T> {
public:
    Consumer(Buffer<T>& new_buffer, T& productivity): Worker<T>(new_buffer, productivity){};
    void run(){
        while (!this->isStopped) {
            std::optional<T> value = this->buffer.pop();
            if (value.has_value()) {
                this->productivity += value.value();
            }
            this->sleep();
        }
    };
    ~Consumer(){
        this->stop();
        this->thread.join();
    };
};

#endif //PRODUCER_CONSUMER_CONSUMER_H
