#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include "buffer.h"
#include "workers.h"


#define SUBSEQUENT_ACCESS_DELAY_MS 150




struct TestParameters {
    int consumer_num;
    int producer_num;

    int delay_random_consume_ms;
    int delay_random_generate_ms;
};


int unit_test(TestParameters test_params) {
    typedef std::function<void()> Task;
    typedef std::function<Task()> TaskGenerator;


    auto buffer = std::make_shared<FairDequeBuffer<Task, SUBSEQUENT_ACCESS_DELAY_MS>>();

    std::cout << "Checking correctness..." << std::endl;
    std::cout << "Number of consumers is " << test_params.consumer_num << ", number of producers is "
              << test_params.producer_num << std::endl;

    std::atomic<int> atomic_task_created{0};
    std::atomic<int> atomic_task_processed{0};

    auto consumers = std::unique_ptr<std::vector<std::shared_ptr<ConsumerWorker<Task>>>>(
            new std::vector<std::shared_ptr<ConsumerWorker<Task>>>);
    auto producers = std::unique_ptr<std::vector<std::shared_ptr<ProducerWorker<Task>>>>(
            new std::vector<std::shared_ptr<ProducerWorker<Task>>>);

    TaskGenerator generator = [&atomic_task_created, &atomic_task_processed, test_params]() {

        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % test_params.delay_random_generate_ms));
        atomic_task_created++;

        return [&atomic_task_processed, test_params]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % test_params.delay_random_consume_ms));
            atomic_task_processed++;
        };
    };

    for (int i = 0; i < test_params.consumer_num; ++i) {
        auto worker = std::make_shared<ConsumerWorker<Task>>(buffer);
        consumers->emplace_back(worker);
    }

    for (int i = 0; i < test_params.producer_num; ++i)
        producers->push_back(std::make_shared<ProducerWorker<Task>>(buffer, generator));

    std::string s;

    std::cout << "Press Any Key To Exit..." << std::endl;
    std::cin >> s;

    for (auto &consumer :*consumers) consumer->cancel();
    for (auto &producer :*producers) producer->cancel();

    int total_finished_consumers = 0;
    int total_finished_producers = 0;

    for (auto &consumer : *consumers) {
        consumer->join();
        total_finished_consumers += int(consumer->is_worker_finish());
    }
    for (auto &producer : *producers) {
        producer->join();
        total_finished_producers += int(producer->is_worker_finish());
    }


    std::cout << "Total created tasks:" << atomic_task_created << std::endl;
    std::cout << "Total processed tasks: " << atomic_task_processed << std::endl;
    std::cout << "Total task that left in buffer: " << buffer->size() << std::endl;
    std::cout << "Total finished consumers: " << total_finished_consumers << std::endl;

    std::cout << "Total finished producers: " << total_finished_producers << std::endl;

    auto result = (total_finished_producers == test_params.producer_num) &&
                  (total_finished_consumers == test_params.consumer_num) &&
                  (atomic_task_created == (atomic_task_processed + buffer->size()));

    std::cout << ((result) ? "Test success" : "Test failed") << std::endl;

    return result;


}


int main(int argc, char *argv[]) {

    auto equal = TestParameters{
            4,
            4,

            50,
            50


    };

    auto more_consumers = TestParameters{
            8,
            4,

            50,
            50


    };
///**/
    auto more_producers = TestParameters{
            4,
            8,

            50,
            50


    };

    auto equal_res = unit_test(equal);
    std::cout << "----------------------------------------------" << std::endl;

    auto cons_res = unit_test(more_consumers);
    std::cout << "----------------------------------------------" << std::endl;

    auto prod_res = unit_test(more_producers);
    std::cout << "----------------------------------------------" << std::endl;


    if (equal_res + cons_res + prod_res == 3) {
        std::cout << "All tests passed";
    } else {
        std::cout << "Some tests failed:(";
    }
    return 0;
}
