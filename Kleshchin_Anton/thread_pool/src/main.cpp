#include "task.hpp"
#include "thread_pool.hpp"
#include <iostream>
#include <cassert>

#define final_assert(e) assert(e); std::cout << "    " << __FUNCTION__ << " passed" << std::endl

using namespace std;

void IntVoidTest(size_t threads = 1) {
    ThreadPool a(threads);
    auto t = MakeTask([]{return 42;});
    auto res = t->GetFuture();
    a.Enqueue(move(t));
    final_assert(res.get() == 42);
}

void VoidVoidTest(size_t threads = 1) {
    ThreadPool a(threads);
    int value = 0;
    auto t = MakeTask([&value]{value = 42;});
    auto res = t->GetFuture();
    a.Enqueue(move(t));
    res.get();
    final_assert(value == 42);
}

void ThrowVoidVoidTest(size_t threads = 1) {
    ThreadPool a(threads);
    auto t = MakeTask([]{throw runtime_error("very bad error");});
    auto res = t->GetFuture();
    a.Enqueue(move(t));

    bool ok = false;
    try {
        res.get();
        assert(false);
    } catch(std::exception& e) {
        ok = (e.what() == "very bad error"sv);
    }
    final_assert(ok);
}

void ThrowIntVoidTest(size_t threads = 1) {
    ThreadPool a(threads);
    auto t = MakeTask([]{throw runtime_error("very bad error"); return 15;});
    auto res = t->GetFuture();
    a.Enqueue(move(t));

    bool ok = false;
    try {
        res.get();
        assert(false);
    } catch(std::exception& e) {
        ok = (e.what() == "very bad error"sv);
    }
    final_assert(ok);
}

void ChainedIntUniqueContinuationTest(size_t threads = 1) {
    ThreadPool a(threads);
    auto sqr = [](int v){ return v * v; };
    auto t = MakeTask([]{return 42;});
    auto& t2 = t->UniqueContinueWith(sqr);
    auto& t3 = t2.UniqueContinueWith(sqr);
    auto res = t3.GetFuture();
    a.Enqueue(move(t));
    final_assert(res.get() == sqr(sqr(42)));
}

void ChainedVoidUniqueContinuationTest(size_t threads = 1) {
    ThreadPool a(threads);
    int v = 0;
    auto sqr = [&v](){ v = v * v; };
    auto t = MakeTask([&v]{v = 42;});
    auto& t2 = t->UniqueContinueWith(sqr);
    auto& t3 = t2.UniqueContinueWith(sqr);
    auto res = t3.GetFuture();
    a.Enqueue(move(t));
    res.get();
    final_assert(v == 42*42*42*42);
}

void ChainedUniqueContinuationThrowTest(size_t threads = 1) {
    ThreadPool a(threads);
    bool ok = false;

    auto t = MakeTask([&ok]{ok = true;});
    auto& t2 = t->UniqueContinueWith([]{ throw runtime_error("very bad error"); });
    auto& t3 = t2.UniqueContinueWith([]{ assert(false); });
    auto res = t3.GetFuture();
    a.Enqueue(move(t));

    try {
        res.get();
        assert(false);
    } catch(std::exception& e) {
        ok &= (e.what() == "very bad error"sv);
    }
    final_assert(ok);
}

void RunAllTests(size_t threads) {
    std::cout << "Started tests with " << threads << " threads" << std::endl;
    IntVoidTest(threads);
    VoidVoidTest(threads);
    ThrowVoidVoidTest(threads);
    ThrowIntVoidTest(threads);
    ChainedIntUniqueContinuationTest(threads);
    ChainedVoidUniqueContinuationTest(threads);
    ChainedUniqueContinuationThrowTest(threads);
    std::cout << "All tests with " << threads << " threads successfully passed" << std::endl;
}

int main() {
    RunAllTests(1);
    RunAllTests(8);
    std::cout << "All tests successfully passed!\n";
    return 0;
}