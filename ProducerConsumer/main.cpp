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
// ���޸ĳ�����Modifiable constants��
int MAX = 10;					// �����������ݵ��������The maximum amount of data that can be produced��
int producer_number = 2;		// �����ߵ�������Number of producers��
int consumer_number = 3;		// �����ߵ�������Number of consumers��
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

mutex m_mutex, number_mutex;	// ���������������data_number����Shared queue lock and number data_number lock��
queue<int> shared_data;			// ���������������ݣ�Producer-produced data��
int data_number = 0;			// ��ʼʱ���е���ĿΪ0��The initial number is 0��

mutex io_mutex;
bool is_over = false;

void produce(int i) {
	while (true && !is_over) {
		// �жϵ�ǰ�Ѿ������˶������ݣ���ȡ���е�������������������� data_number ������Determine how much data has been produced currently, read the existing quantity, and lock the quantity shared variable data_number��
		unique_lock<mutex> number_lock(number_mutex);
		if (data_number < MAX) {
			// �����������ݵ�����С�����ֵʱ�������������ݣ�When the number of produced data is less than the maximum value, data can be produced��
			// ����Ź������ݵĶ��� shared_data ������Lock the queue shared_data that stores shared data��
			unique_lock<mutex> p_lock(m_mutex);
			// �������ݣ�Production Data��
			int data = rand() % 100;
			shared_data.push(data);
			++data_number;	// ��ǰ������1��Current quantity plus 1��
			{
				unique_lock<mutex> io_lock(io_mutex);
				cout << "Producer " << i << " produce a data : " << data << ", There are currently  " << data_number << " data remaining in total." << endl;				
			}
			// ˯��2���ʾ�������������У�Sleeping for 2 seconds means that data is being produced��
			this_thread::sleep_for(chrono::seconds(2));
		}
	}
	unique_lock<mutex> io_lock(io_mutex);
	cout << "Producer " << i << "stopped production" << endl;
}

void consume(int i) {
	while (true && !is_over) {
		// �жϵ�ǰ�Ѿ������˶������ݣ���ȡ���е�������������������� data_number ������Determine how much data has been produced currently, read the existing quantity, and lock the quantity shared variable data_number��
		unique_lock<mutex> number_lock(number_mutex);
		if (data_number > 0) {
			// �����������ݵ���������0ʱ�������������ݣ�When the amount of data produced is greater than 0, data can be consumed��
			// ����Ź������ݵĶ��� shared_data ������Lock the queue shared_data that stores shared data��
			unique_lock<mutex> p_lock(m_mutex);
			// ȡ���ݽ������ѣ�Fetch data for consumption��
			int data = shared_data.front();
			shared_data.pop();
			--data_number;	// ��ǰ������1��Current quantity minus 1��
			{
				unique_lock<mutex> io_lock(io_mutex);
				cout << "consumer" << i << " consuming a piece of data : " << data << ", currently remaining " << data_number << " data." << endl;
			}
			// ˯��4���ʾ�������������У�Sleeping for 4 seconds means that you are consuming data��
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

	// ��ʼ�������е������ߣ�Start creating all producers��
	for (int i = 0; i < producer_number; ++i) {
		unique_lock<mutex> io_lock(io_mutex);
		cout << "create" << i + 1 << "producer" << endl;
		producer_threads.push_back(thread(produce, i + 1));
	}

	// ��ʼ�������е������ߣ�Start creating all consumers��
	for (int i = 0; i < consumer_number; ++i) {
		unique_lock<mutex> io_lock(io_mutex);
		cout << "create" << i + 1 << "consumer" << endl;
		consumer_threads.push_back(thread(consume, i + 1));
	}

	if (stop_thread.joinable())
		stop_thread.join();
	// �����߿�ʼ������Producer starts to work��
	for (auto & t : producer_threads) {
		if (t.joinable())
			t.join();
	}
	cout << "All producers have stopped production" << endl;

	// �����߿�ʼ������Consumers get to work��
	for (auto & t : consumer_threads) {
		if (t.joinable())
			t.join();
	}
	cout << "All consumers have stopped consumption" << endl;

	system("pause");

	return 0;
}