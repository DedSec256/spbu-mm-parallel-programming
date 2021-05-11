//
// Created by garrancha on 01.05.2021.
// Some utils classes
//

#ifndef TASK2_UTILS_H
#define TASK2_UTILS_H


class NoResetEvent {

public:
    std::atomic<bool> state;

    NoResetEvent() : state(false) {}

    NoResetEvent(const NoResetEvent &other) = delete;

    void WaitOne() {
        std::unique_lock<std::mutex> lock(sync);
        underlying.wait(lock, [this]() { return state.load(std::memory_order_seq_cst); });
    }

    void Set() {
        std::unique_lock<std::mutex> lock(sync);
        state.store(true,std::memory_order_seq_cst);
        underlying.notify_all();
    }

private:
    std::condition_variable underlying;
    std::mutex sync;
};


class TaskException : public std::exception {
private:
    std::string message_;
public:
    explicit TaskException(std::string message) : std::exception(), message_(std::move(message)) {}


    const char *what() const noexcept override {
        return message_.c_str();
    }
};



template<class R, class ... Args>
class FuncWrapper {

    std::function<R(Args...)> function;
public:
    explicit FuncWrapper(std::function<R(Args...)> f) : function(std::move(f)) {}

    R operator()(Args ... args) const {
        return std::move(function(args ...));
    }
};

template<typename ... Args>
class FuncWrapper<void,Args ...> {

    std::function<void(Args...)> function;
public:
    explicit FuncWrapper(std::function<void(Args...)> f) : function(std::move(f)) {}

    void operator()(Args ... args) const {
        function(args ...);
    }
};


#endif //TASK2_UTILS_H
