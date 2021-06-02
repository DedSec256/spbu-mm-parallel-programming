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

	// �����µ��߳̿�ʼ����
	void startWork() {
		work_thread.reset(new std::thread(std::bind(&Worker::run, this)));
	}

	// �������������ñ�־λ�����߳��ڵ�ѭ��
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
		// ѭ����һֱ������ֱ���û�ָ���˳�
		while (!work_over.load()) {
			work();
		}
	}

private:
	std::atomic<bool> work_over{ false };
	std::shared_ptr<std::thread> work_thread;

};

#endif //PRODUCER_CONSUMER_WORKERS_H
