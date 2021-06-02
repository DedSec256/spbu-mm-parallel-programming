//
// Created by 19216 on 2021/5/22.
//

#ifndef PRODUCER_CONSUMER_CONSUMER_H
#define PRODUCER_CONSUMER_CONSUMER_H

#include "worker.h"
#include <functional>

template <typename T>
class Consumer : public Worker<T> {

public:
	explicit Consumer(std::shared_ptr<JobQueue<T>> jobs) :
		Worker<T>(jobs) {
		this->startWork();
	}

	void work() override {
		T t;
		bool result = this->jobsPtr->pop(t);
		if (result) {
			job_number++;
			t();
		}
	}

	size_t getConsumedJobNumber() const { return job_number; }

private:
	std::function<T()> consume;
	std::atomic<size_t> job_number{ 0 };          // 已经消费的任务数量
};

#endif //PRODUCER_CONSUMER_CONSUMER_H
