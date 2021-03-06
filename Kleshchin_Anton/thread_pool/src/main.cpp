#include "task.hpp"
#include "thread_pool.hpp"
#include <iostream>
#include <chrono>
#include <cassert>
#include <random>
#include <algorithm>

#define final_assert(e) assert(e); std::cout << "    " << __FUNCTION__ << " passed" << std::endl

using namespace std;

void IsCompleteTest(size_t threads) {
    ThreadPool a(threads);
    auto t = MakeTask([]{return 42;});
    auto res = t->GetFuture();
    assert(res.wait_for(0s) != future_status::ready);
    a.Enqueue(move(t));
    res.wait();
    assert(res.wait_for(0s) == future_status::ready); // this statement shows how to check that a task "IsComplete".
    final_assert(res.get() == 42);
}

void IntVoidTest(size_t threads) {
    ThreadPool a(threads);
    auto t = MakeTask([]{return 42;});
    auto res = t->GetFuture();
    a.Enqueue(move(t));
    final_assert(res.get() == 42);
}

void VoidVoidTest(size_t threads) {
    ThreadPool a(threads);
    int value = 0;
    auto t = MakeTask([&value]{value = 42;});
    auto res = t->GetFuture();
    a.Enqueue(move(t));
    res.get();
    final_assert(value == 42);
}

void ThrowVoidVoidTest(size_t threads) {
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

void ThrowIntVoidTest(size_t threads) {
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

void ChainedIntUniqueContinuationTest(size_t threads) {
    ThreadPool a(threads);
    auto sqr = [](int v){ return v * v; };
    auto t = MakeTask([]{return 42;});
    auto& t2 = t->UniqueContinueWith(sqr);
    auto& t3 = t2.UniqueContinueWith(sqr);
    auto res = t3.GetFuture();
    a.Enqueue(move(t));
    final_assert(res.get() == sqr(sqr(42)));
}

void ChainedVoidUniqueContinuationTest(size_t threads) {
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

void ChainedUniqueContinuationThrowTest(size_t threads) {
    ThreadPool a(threads);
    bool ok1 = false;
    bool ok2 = false;

    auto t = MakeTask([&ok1]{ok1 = true;});
    auto& t2 = t->UniqueContinueWith([]{ throw runtime_error("very bad error"); });
    auto& t3 = t2.UniqueContinueWith([]{ assert(false); });
    auto res = t3.GetFuture();
    a.Enqueue(move(t));

    try {
        res.get();
        assert(false);
    } catch(std::exception& e) {
        ok2 = (e.what() == "very bad error"sv);
    }
    final_assert(ok1 && ok2);
}

void ChainedIntSharedContinuationTest(size_t threads) {
    ThreadPool a(threads);
    auto sqr = [](int v){ return v * v; };
    auto sum = [](int v){ return v + v + v; };
    auto t = MakeTask([]{return 42;});

    auto& ta = t->SharedContinueWith(sqr);
    auto& tb = t->SharedContinueWith(sum);
    auto& t1 = ta.UniqueContinueWith(sum);
    auto& t2 = tb.UniqueContinueWith(sqr);
    auto r1 = t1.GetFuture();
    auto r2 = t2.GetFuture();

    a.Enqueue(move(t));

    final_assert(r1.get() == sum(sqr(42)) && r2.get() == sqr(sum(42)));
}

void ChainedSharedContinuationThrowTest(size_t threads) {
    ThreadPool a(threads);
    bool ok = false;
    auto never_call = []{ assert(false); };

    auto t = MakeTask([&ok]{ok = true;});
    auto& t1 = t->SharedContinueWith([]{ throw runtime_error("very bad error"); });
    auto& t2 = t->SharedContinueWith([]{ throw runtime_error("very good error"); });
    auto& t1a = t1.SharedContinueWith(never_call);
    auto& t1b = t1.SharedContinueWith(never_call);
    auto& t2a = t2.SharedContinueWith(never_call);
    auto& t2b = t2.SharedContinueWith(never_call);
    auto r1a = t1a.GetFuture();
    auto r1b = t1b.GetFuture();
    auto r2a = t2a.GetFuture();
    auto r2b = t2b.GetFuture();
    a.Enqueue(move(t));


    auto throw_checker = [](auto & r, auto const & err_msg) {
        bool ok = false;
        try {
            r.get();
            assert(false);
        } catch(std::exception& e) {
            ok = (e.what() == err_msg);
        }
        assert(ok);
    };
    throw_checker(r1a, "very bad error"sv);
    throw_checker(r1b, "very bad error"sv);
    throw_checker(r2a, "very good error"sv);
    throw_checker(r2b, "very good error"sv);
    final_assert(ok);
}

void ManyTaskTest(size_t threads) {
    ThreadPool a(threads);
    bool ok = false;
    auto root = MakeTask([&ok]{ ok = true; return 0ull;});

    constexpr size_t n = 1000;
    constexpr size_t m = 10000;
    vector<future<size_t>> ans;
    ans.reserve(n*m);

    for (size_t i = 0; i < n; ++i) {
        auto& t = root->SharedContinueWith([i](size_t v){ return i + v; });
        for (size_t j = 0; j < m; ++j) {
            auto& tt = t.SharedContinueWith([j](size_t v){ return j * v; });
            ans.push_back(tt.GetFuture());
        }
    }

    a.Enqueue(move(root));
    
    size_t index = 0;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j)
            assert(ans[index++].get() == i*j);
    }
    final_assert(ok && index == ans.size());
}

void QuickParallelSort(ThreadPool & tp, vector<size_t>::iterator from, vector<size_t>::iterator to) {
    if (to - from <= 1000) {
        std::sort(from, to);
        return;
    }
    auto mid = partition(from, to, [pivot = *from](auto e) {return e < pivot;});
    mid += (mid == from);
    auto right_part = MakeTask([&tp, mid, to]{ QuickParallelSort(tp, mid, to); });
    auto res = right_part->GetFuture();
    tp.Enqueue(move(right_part));
    QuickParallelSort(tp, from, mid);
    while (res.wait_for(0s) != future_status::ready)
        tp.RunPendingTask();
}

void SortTest(size_t threads) {
    ThreadPool tp(threads);
    constexpr size_t n = 1e7;
    vector<size_t> v(n);
    for (size_t i = 0; i < n; ++i)
        v[i] = i;

    do {
        shuffle(v.begin(), v.end(), mt19937());
    } while(is_sorted(v.begin(), v.end()));

    QuickParallelSort(tp, v.begin(), v.end());

    final_assert(is_sorted(v.begin(), v.end()));
}

void RunAllTests(size_t threads) {
    cout << "Started tests with " << threads << " threads" << endl;
    IntVoidTest(threads);
    VoidVoidTest(threads);
    ThrowVoidVoidTest(threads);
    ThrowIntVoidTest(threads);
    IsCompleteTest(threads);
    ChainedIntUniqueContinuationTest(threads);
    ChainedVoidUniqueContinuationTest(threads);
    ChainedUniqueContinuationThrowTest(threads);
    ChainedIntSharedContinuationTest(threads);
    ChainedSharedContinuationThrowTest(threads);
    ManyTaskTest(threads);
    SortTest(threads);
    cout << "All tests with " << threads << " threads successfully passed" << endl;
}

int main() {
    RunAllTests(1);
    RunAllTests(8);
    cout << "All tests successfully passed!\n";
    return 0;
}