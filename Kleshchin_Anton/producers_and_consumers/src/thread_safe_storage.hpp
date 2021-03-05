#pragma once
#include <queue>
#include <mutex>

template<class T>
class TSQueue {
    std::queue<T> storage;
    std::mutex mt;
public:

    std::queue<T> & RawStorage() noexcept {
        return storage;
    }

    bool Empty() const noexcept {
        std::lock_guard guard(mt);
        return storage.empty();
    }

    template<class U>
    void Put(U && value) {
        std::lock_guard guard(mt);
        storage.push(std::forward<U>(value));
    }

    bool Take(T & res) {
        std::lock_guard guard(mt);
        if (storage.empty())
            return false;
        res = std::move(storage.front());
        storage.pop();
        return true;
    }
};
