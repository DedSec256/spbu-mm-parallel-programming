#include <iostream>
#include "Buffer.h"
#include "Worker.h"
#include "TestRunner.h"
#include "Consumer.h"
#include "Producer.h"
#include <memory>

int generator(){
    return rand() % 100;
}

std::pair<std::vector<int>, std::vector<int>> run(Buffer<int>& buffer, int numOfProds, int numOfCons) {
    std::vector<std::unique_ptr<Consumer<int>>> consumers;
    std::vector<std::unique_ptr<Producer<int>>> producers;

    std::vector<int> producersProductivity(numOfProds, 0);
    std::vector<int> consumersProductivity(numOfCons, 0);

    producers.reserve(numOfProds);
    for (int i = 0; i < numOfProds; i++)
        producers.push_back(std::make_unique<Producer<int>>(buffer, generator, producersProductivity[i]));

    consumers.reserve(numOfCons);
    for (int i = 0; i < numOfCons; i++) {
        consumers.push_back(std::make_unique<Consumer<int>>(buffer, consumersProductivity[i]));
    }

    std::cout << "Press any key to stop" << std::endl;
    std::cin.get();

    for (auto &prod: producers)
        prod->stop();

    for (auto &cons: consumers)
        cons->stop();

    return {producersProductivity, consumersProductivity};
}

bool check(Buffer<int>& buffer, const std::vector<int>& producersProductivity, const std::vector<int>& consumersProductivity){
    int totalProdProductivity = 0, totalConsProductivity = 0;

    for(auto prods: producersProductivity){
        totalProdProductivity += prods;
    }

    for(auto cons: consumersProductivity){
        totalConsProductivity += cons;
    }
    std::cout << "Total producers sum " << totalProdProductivity << std::endl;
    std::cout << "Total consumers sum " << totalConsProductivity << std::endl;

    while (!buffer.empty()) {
        totalConsProductivity += buffer.pop().value();
    }

    std::cout << "Total consumers sum with the buffer remainder " << totalConsProductivity << std::endl;

    return totalConsProductivity == totalProdProductivity;
}

bool RunProducerAndConsumer(int numOfProds, int numOfCons){
    Buffer<int> buffer;
    auto productivity = run(buffer, numOfProds, numOfCons);
    return check(buffer, productivity.first, productivity.second);
}

void TestZeroConsumers(){
    std::cout << "Starting test \'TestZeroConsumers\'" << std::endl;
    {
        Assert(RunProducerAndConsumer(10, 0), "Test with zero consumers");
    }

}

void TestZeroProducers(){
    std::cout << "Starting test \'TestZeroProducers\'" << std::endl;
    {
        Assert(RunProducerAndConsumer(0, 13), "Test with zero producers");
    }

}

void TestNonzeroConsumersAndProducers(){
    std::cout << "Starting test \'TestNonzeroConsumersAndProducers\'" << std::endl;
    {
        Assert(RunProducerAndConsumer(50, 15), "Test with 50 producers and 15 consumers");
    }
}

void CustomTest(){
    int numOfProds, numOfCons;

    std::cout << "Enter number of producers for Custom Test: " << std::endl;
    std::cin >> numOfProds;

    std::cout << "Enter number of consumers for Custom Test: " << std::endl;
    std::cin  >> numOfCons;

    std::cout << "Starting test \'CustomTest\'" << std::endl;
    {
        Assert(RunProducerAndConsumer(numOfProds, numOfCons), "Custom Test");
    }

}

void TestAll(){
    TestRunner tr;
    tr.RunTest(TestZeroProducers, "TestZeroProducers");
    tr.RunTest(TestZeroConsumers, "TestZeroConsumers");
    tr.RunTest(TestNonzeroConsumersAndProducers, "TestNonzeroConsumersAndProducers");
//    tr.RunTest(CustomTest, "CustomTest");
}

int main() {
    TestAll();
    return 0;
}
