#pragma once
#include <queue>
#include <mutex>

template<class T>
class TSQueue {
    std::queue<T> storage;
    std::mutex mt;
public:

    bool Empty() const noexcept {
        std::lock_guard guard(mt);
        return storage.empty();
    }

    template<class U>
    void Push(U && value) {
        std::lock_guard guard(mt);
        storage.push(std::forward<U>(value));
    }

    bool TryPop(T & res) {
        std::lock_guard guard(mt);
        if (storage.empty())
            return false;
        res = std::move(storage.front());
        storage.pop();
        return true;
    }
};
