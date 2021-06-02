//
// Created by 19216 on 2021/5/22.
//

#ifndef PRODUCER_CONSUMER_PRODUCER_H
#define PRODUCER_CONSUMER_PRODUCER_H

#include "worker.h"
#include <functional>

template <typename T>
class Producer : public Worker<T> {

public:

	explicit Producer(std::shared_ptr<JobQueue<T>> jobs, std::function<T()> produce) :
		Worker<T>(jobs), produce(produce) {
		this->startWork();
	}

	void work() override {
		auto consume = produce();
		job_number++;
		this->jobsPtr->push(consume);
	}

	~Producer() {
		this->join();
	}

	size_t getProducedJobNumber() const { return job_number; }

private:
	std::function<T()> produce;
	std::atomic<size_t> job_number{ 0 };          // 生产的任务数量
};

#endif //PRODUCER_CONSUMER_PRODUCER_H
