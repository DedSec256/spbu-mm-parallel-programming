//
// Created by 19216 on 2021/5/22.
//

#ifndef PRODUCER_CONSUMER_JOB_QUEUE_H
#define PRODUCER_CONSUMER_JOB_QUEUE_H

#include <queue>
#include <mutex>

template<typename T>
class JobQueue {

public:
	void push(T t) {
		std::unique_lock<std::mutex> lock(queue_mutex);
		jobs.push(std::move(t));
		//        std::cout << "after push jobs size : " << jobs.size() << std::endl;
	}

	bool pop(T &t) {
		std::unique_lock<std::mutex> lock(queue_mutex);
		if (jobs.empty()) return false;
		t = std::move(jobs.front());
		jobs.pop();
		//        std::cout << "after pop jobs size : " << jobs.size() << std::endl;
		return true;
	}

	size_t size() {
		std::unique_lock<std::mutex> lock(queue_mutex);
		return jobs.size();
	}

private:
	std::queue<T> jobs;
	std::mutex queue_mutex;
};


#endif //PRODUCER_CONSUMER_JOB_QUEUE_H
