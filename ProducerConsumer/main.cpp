#include <iostream>
#include <mutex>
#include <queue>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
using namespace std;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
// 可修改常量（Modifiable constants）
int MAX = 10;					// 可以生产数据的最大量（The maximum amount of data that can be produced）
int producer_number = 2;		// 生产者的数量（Number of producers）
int consumer_number = 3;		// 消费者的数量（Number of consumers）
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

mutex m_mutex, number_mutex;	// 共享队列锁和数量data_number锁（Shared queue lock and number data_number lock）
queue<int> shared_data;			// 生产者生产的数据（Producer-produced data）
int data_number = 0;			// 初始时已有的数目为0（The initial number is 0）

mutex io_mutex;
bool is_over = false;

void produce(int i) {
	while (true && !is_over) {
		// 判断当前已经生产了多少数据，读取已有的数量，对数量共享变量 data_number 加锁（Determine how much data has been produced currently, read the existing quantity, and lock the quantity shared variable data_number）
		unique_lock<mutex> number_lock(number_mutex);
		if (data_number < MAX) {
			// 当生产的数据的数量小于最大值时，可以生产数据（When the number of produced data is less than the maximum value, data can be produced）
			// 将存放共享数据的队列 shared_data 加锁（Lock the queue shared_data that stores shared data）
			unique_lock<mutex> p_lock(m_mutex);
			// 生产数据（Production Data）
			int data = rand() % 100;
			shared_data.push(data);
			++data_number;	// 当前数量加1（Current quantity plus 1）
			{
				unique_lock<mutex> io_lock(io_mutex);
				cout << "Producer " << i << " produce a data : " << data << ", There are currently  " << data_number << " data remaining in total." << endl;				
			}
			// 睡眠2秒表示正在生产数据中（Sleeping for 2 seconds means that data is being produced）
			this_thread::sleep_for(chrono::seconds(2));
		}
	}
	unique_lock<mutex> io_lock(io_mutex);
	cout << "Producer " << i << "stopped production" << endl;
}

void consume(int i) {
	while (true && !is_over) {
		// 判断当前已经生产了多少数据，读取已有的数量，对数量共享变量 data_number 加锁（Determine how much data has been produced currently, read the existing quantity, and lock the quantity shared variable data_number）
		unique_lock<mutex> number_lock(number_mutex);
		if (data_number > 0) {
			// 当生产的数据的数量大于0时，可以消费数据（When the amount of data produced is greater than 0, data can be consumed）
			// 将存放共享数据的队列 shared_data 加锁（Lock the queue shared_data that stores shared data）
			unique_lock<mutex> p_lock(m_mutex);
			// 取数据进行消费（Fetch data for consumption）
			int data = shared_data.front();
			shared_data.pop();
			--data_number;	// 当前数量减1（Current quantity minus 1）
			{
				unique_lock<mutex> io_lock(io_mutex);
				cout << "consumer" << i << " consuming a piece of data : " << data << ", currently remaining " << data_number << " data." << endl;
			}
			// 睡眠4秒表示正在消费数据中（Sleeping for 4 seconds means that you are consuming data）
			this_thread::sleep_for(chrono::seconds(4));
		}
	}
	unique_lock<mutex> io_lock(io_mutex);
	cout << "consumer" << i << " stopped consumption" << endl;
}

void stop() {
	char input;
	cin >> input;
	//while (input != 'q') 
	//	cin >> input;
	unique_lock<mutex> io_lock(io_mutex);
	cout << "The thread is stopping..." << endl;
	is_over = true;
}

int main(int argc, char * argv[]) {

	vector<thread> producer_threads, consumer_threads;
	thread stop_thread(stop);

	// 开始创建所有的生产者（Start creating all producers）
	for (int i = 0; i < producer_number; ++i) {
		unique_lock<mutex> io_lock(io_mutex);
		cout << "create" << i + 1 << "producer" << endl;
		producer_threads.push_back(thread(produce, i + 1));
	}

	// 开始创建所有的消费者（Start creating all consumers）
	for (int i = 0; i < consumer_number; ++i) {
		unique_lock<mutex> io_lock(io_mutex);
		cout << "create" << i + 1 << "consumer" << endl;
		consumer_threads.push_back(thread(consume, i + 1));
	}

	if (stop_thread.joinable())
		stop_thread.join();
	// 生产者开始工作（Producer starts to work）
	for (auto & t : producer_threads) {
		if (t.joinable())
			t.join();
	}
	cout << "All producers have stopped production" << endl;

	// 消费者开始工作（Consumers get to work）
	for (auto & t : consumer_threads) {
		if (t.joinable())
			t.join();
	}
	cout << "All consumers have stopped consumption" << endl;

	system("pause");

	return 0;
}