//
// Created by 19216 on 2021/5/22.
//

#include <iostream>
#include <chrono>
#include <vector>
#include "producer.h"
#include "consumer.h"

using namespace std;

// 消费的任务
void consume_task() {
	//    cout << "consuming ..." << endl;
	this_thread::sleep_for(chrono::microseconds(500) * (rand() % 500));
}

// 生产任务
function<void()> produce() {
	//    cout << "producing ..." << endl;
	this_thread::sleep_for(chrono::microseconds(500) * (rand() % 500));
	return consume_task;
}

void test(int producer_number, int consumer_number) {

	// 任务的类型
	using JOB_TYPE = decltype(produce());

	// 任务队列
	shared_ptr<JobQueue<JOB_TYPE>> jobQueue = make_shared<JobQueue<JOB_TYPE>>();

	// 生产者数组和消费者数组
	vector<shared_ptr<Producer<JOB_TYPE>>> producers;
	vector<shared_ptr<Consumer<JOB_TYPE>>> consumers;

	// 创建生产者
	for (int i = 0; i < producer_number; ++i) {
		producers.push_back(std::make_shared<Producer<JOB_TYPE>>(jobQueue, produce));
	}

	// 创建消费者
	for (int i = 0; i < consumer_number; ++i) {
		consumers.push_back(make_shared<Consumer<JOB_TYPE>>(jobQueue));
	}

	cout << "Please press any key to stop this test ..." << endl;
	cin.get();

	for (auto & producer : producers) {
		producer->workOver();
	}

	for (auto &consumer : consumers) {
		consumer->workOver();
	}

	size_t producedJobs = 0, consumedJobs = 0;
	for (auto &producer : producers) {
		producer->join();
		producedJobs += producer->getProducedJobNumber();
	}
	for (auto &consumer : consumers) {
		consumer->join();
		consumedJobs += consumer->getConsumedJobNumber();
	}

	cout << "#################################\n";
	cout << "producer number : " << producer_number << endl;
	cout << "consumer number : " << consumer_number << endl;
	cout << producer_number << " producers product " << producedJobs << " jobs" << endl;
	cout << consumer_number << " consumers consume " << consumedJobs << " jobs" << endl;
	cout << "There are " << jobQueue->size() << " jobs rest in job queue" << endl;
	cout << "consumed job number( " << consumedJobs << " ) + rest job number( " << jobQueue->size() << " )";
	cout << ((consumedJobs + jobQueue->size() == producedJobs) ? " == " : " != ");
	cout << "produced job number( " << producedJobs << " )" << endl;
	cout << ((consumedJobs + jobQueue->size() == producedJobs) ? "Test passes successful!" : "Test notPass failed!") << endl;
	cout << "#################################\n" << endl;
}

int main(int argc, char* argv[]) {

	try {
		test(5, 5);
		test(7, 5);
		test(5, 7);
	}
	catch (exception &e) {
		cout << "Some error occur : " << e.what() << endl;
	}

	return 0;
}