//
// Created by 19216 on 2021/5/22.
//

#ifndef PRODUCER_CONSUMER_WORKERS_H
#define PRODUCER_CONSUMER_WORKERS_H

#include "job_queue.h"
#include <atomic>
#include <memory>
#include <thread>
#include <functional>

template <typename T>
class Worker {

public:
	explicit Worker(std::shared_ptr<JobQueue<T>> jobs) : jobsPtr(jobs) {
	}

	// 开启新的线程开始工作
	void startWork() {
		work_thread.reset(new std::thread(std::bind(&Worker::run, this)));
	}

	// 工作结束，设置标志位结束线程内的循环
	void workOver() {
		work_over.store(true);
	}

	void join() {
		work_over.store(true);
		if (work_thread->joinable())
			work_thread->join();
	}

	virtual ~Worker() {
		if (work_thread->joinable())
			work_thread->join();
	}

protected:
	std::shared_ptr<JobQueue<T>> jobsPtr;
	virtual void work() = 0;

private:

	void run() {
		// 循环内一直工作，直到用户指定退出
		while (!work_over.load()) {
			work();
		}
	}

private:
	std::atomic<bool> work_over{ false };
	std::shared_ptr<std::thread> work_thread;

};

#endif //PRODUCER_CONSUMER_WORKERS_H
