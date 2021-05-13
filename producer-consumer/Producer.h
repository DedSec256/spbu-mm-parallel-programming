//
// Created by Мунира on 08.05.2021.
//

#include "Worker.h"
#include "Buffer.h"

#ifndef PRODUCER_CONSUMER_PRODUCER_H
#define PRODUCER_CONSUMER_PRODUCER_H


template<typename T>
class Producer : public Worker<T> {
public:
    Producer(Buffer<T>& new_buffer, std::function<T()> new_generator,
             T& productivity): Worker<T>(new_buffer, productivity), generator(new_generator){};
    void run(){
        while (!this->isStopped) {
            T value = generator();
            this->buffer.push(value);
            this->productivity += value;
            this->sleep();
        }
    };
    ~Producer() {
        this->stop();
        this->thread.join();
    };
private:
    std::function<T()> generator;
};


#endif //PRODUCER_CONSUMER_PRODUCER_H
