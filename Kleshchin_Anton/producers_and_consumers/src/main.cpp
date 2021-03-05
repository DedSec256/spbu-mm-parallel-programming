#include "workers.hpp"

#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <list>

using namespace std;

pair<size_t, size_t> GetAmountOfProdsAndCons(int argc, char const *argv[]) {
    if (argc != 3)
        throw "Usage: "s + argv[0] + " <amount of produsers> <amount of consumers>";

    return {stoull(argv[1]), stoull(argv[2])};
}

template<class T>
list<WorkerThread<T>> RunAllWorkers(list<T> & workers) {
    list<WorkerThread<T>> res;
    for (auto & worker : workers)
        res.emplace_back(ref(worker));
    return res;
}

template<class T>
void SendStopToAllWorkers(list<T> & workers) {
    for (auto & worker : workers)
        worker.Stop();
}

struct WorkerStats {
    size_t calls = 0;
    long long sum = 0;

    void operator+=(WorkerStats const & other) noexcept {
        calls += other.calls;
        sum += other.sum;
    }

    bool operator!=(WorkerStats const & other) const noexcept{
        return calls != other.calls || sum != other.sum;
    }
};

struct ProducerCore {
    mt19937 random_generator;
    WorkerStats & stat;
    int operator()() {
        int value = static_cast<int>(random_generator());
        ++stat.calls;
        stat.sum += value;
        return value;
    }
    ProducerCore(WorkerStats & stat)
        : random_generator(random_device{}())
        , stat(stat)
    {}
};

struct ConsumerCore {
    WorkerStats & stat;
    void operator()(int value) {
        ++stat.calls;
        stat.sum += value;
    }
};

struct Tester {
    TSQueue<int> common_storage;
    vector<WorkerStats> prods_stats;
    vector<WorkerStats> cons_stats;

    Tester(size_t nprods, size_t ncons)
        : prods_stats(nprods)
        , cons_stats(ncons)
    {}

    void PerformTest() {
        list<Producer<int>> prods;
        list<Consumer<int>> cons;
        for (size_t i = 0; i < prods_stats.size(); ++i)
            prods.emplace_back(common_storage, ProducerCore{prods_stats[i]});
        for (size_t i = 0; i < cons_stats.size(); ++i)
            cons.emplace_back(common_storage, ConsumerCore{cons_stats[i]});

        auto prods_threads = RunAllWorkers(prods);
        auto cons_threads = RunAllWorkers(cons);

        cout << "Press enter to stop the test" << endl;
        cin.get();

        SendStopToAllWorkers(prods);
        SendStopToAllWorkers(cons);
    }

    void VerifyCorrectness() {
        WorkerStats total_prods_stat;
        WorkerStats total_cons_stat;

        for (auto const & stat : prods_stats)
            total_prods_stat += stat;

        for (auto const & stat : cons_stats)
            total_cons_stat += stat;

        auto & raw_queue = common_storage.RawStorage();
        while (!raw_queue.empty()) {
            ++total_cons_stat.calls;
            total_cons_stat.sum += raw_queue.front();
            raw_queue.pop();
        }

        auto info = "Producers generated "s + to_string(total_prods_stat.calls) + " elements\n"
                    "    with total sum = " + to_string(total_prods_stat.sum) + "\n"
                    "Consumers got " + to_string(total_cons_stat.calls) + " elements\n"
                    "    with total sum = " + to_string(total_cons_stat.sum);

        if (total_prods_stat != total_cons_stat)
            throw "Test has been failed!\n"s + info;
        
        cout << "Test has been passed!\n" << info << endl;
    }
};

int main(int argc, char const *argv[]) {
    try {
        auto [nprods, ncons] = GetAmountOfProdsAndCons(argc, argv);

        Tester tester(nprods, ncons);
        tester.PerformTest();
        tester.VerifyCorrectness();

    } catch (string & s) {
        cerr << s << '\n';
        return 1;
    } catch (exception & e) {
        cerr << e.what() << '\n';
        return 1;
    } catch (...) {
        cerr << "Caught unknown error, sorry :(\n";
        return 1;
    }
    return 0;
}
