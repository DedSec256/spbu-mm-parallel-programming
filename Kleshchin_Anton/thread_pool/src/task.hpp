#pragma once
#include <atomic>
#include <optional>
#include <future>
#include <vector>

namespace internal {

template<class Arg, bool shared, bool has_value>
struct ArgHolder {
    std::future<Arg> arg;
};

template<class Arg>
struct ArgHolder<Arg, true, true> {
    std::shared_future<Arg> arg;
};

template<class Arg, bool shared>
struct ArgHolder<Arg, shared, false> {
};

template<class R, class Arg>
struct ActionHolder {
    std::function<R(Arg)> action;
};

template<class R>
struct ActionHolder<R, void> {
    std::function<R()> action;
};

} // namespace internal

struct Callable {
    using ContinuationList = std::vector<std::unique_ptr<Callable>>;
    ContinuationList continuations;

    virtual void operator()() noexcept = 0;
    virtual ~Callable() = default;
};

template<class R, class Arg = void, bool is_continuation = false, bool shared = false>
class Task : public Callable, internal::ArgHolder<Arg, shared, is_continuation>, internal::ActionHolder<R, Arg> {
    std::promise<R> result;
    std::optional<std::shared_future<R>> shared_future;
public:

    std::future<R> GetFuture() {
        return result.get_future();
    }

    std::shared_future<R> GetSharedFuture() {
        if (!shared_future)
            shared_future = GetFuture().share();;

        return *shared_future;
    }

    void operator()() noexcept override;

    template<class F>
    auto& UniqueContinueWith(F && f);

    template<class F>
    auto& SharedContinueWith(F && f);

    template<class F>
    Task(F && f) {
        this->action = std::forward<F>(f);
        static_assert(!is_continuation && std::is_same_v<Arg,void> && !shared);
    }

    template<class F>
    Task(F && f, std::future<Arg> && dep_arg) {
        this->action = std::forward<F>(f);
        this->arg = std::move(dep_arg);
        static_assert(is_continuation && !shared);
    }

    template<class F>
    Task(F && f, std::shared_future<Arg> && dep_arg) {
        this->action = std::forward<F>(f);
        this->arg = std::move(dep_arg);
        static_assert(is_continuation && shared);
    }
};

//=====<deduction guides>=====
template<class F>
Task(F && f) -> Task<std::invoke_result_t<F>>;

template<class F>
Task(F && f, std::future<void> && dep_arg) -> Task<std::invoke_result_t<F>, void, true>;

template<class F, class Arg>
Task(F && f, std::future<Arg> && dep_arg) -> Task<std::invoke_result_t<F, Arg>, Arg, true>;

template<class F>
Task(F && f, std::shared_future<void> && dep_arg) -> Task<std::invoke_result_t<F>, void, true, true>;

template<class F, class Arg>
Task(F && f, std::shared_future<Arg> && dep_arg) -> Task<std::invoke_result_t<F, Arg>, Arg, true, true>;
//=====</deduction guides>=====


template<class ... Args>
auto MakeTask(Args && ... args) {
    return std::make_unique<decltype(Task(std::forward<Args>(args)...))>(std::forward<Args>(args)...);
}

template<class R, class Arg, bool is_continuation, bool shared>
template<class F>
auto& Task<R, Arg, is_continuation, shared>::UniqueContinueWith(F && f) {
    auto task = MakeTask(std::forward<F>(f), GetFuture());
    auto res = task.get();
    continuations.emplace_back(std::move(task));
    return *res;
}

template<class R, class Arg, bool is_continuation, bool shared>
template<class F>
auto& Task<R, Arg, is_continuation, shared>::SharedContinueWith(F && f) {
    auto task = MakeTask(std::forward<F>(f), GetSharedFuture());
    auto res = task.get();
    continuations.emplace_back(std::move(task));
    return *res;
}

template<class R, class Arg, bool is_continuation, bool shared>
void Task<R, Arg, is_continuation, shared>::operator()() noexcept {
    try {
        constexpr bool void_arg = std::is_same_v<Arg, void>;
        constexpr bool void_R = std::is_same_v<R, void>;
        
        if constexpr (void_arg) {
            if constexpr (is_continuation)
                this->arg.get();

            if constexpr (!void_R) {
                result.set_value(this->action());
            } else {
                this->action();
                result.set_value();
            }
        } else {
            static_assert(is_continuation);
            if constexpr (!void_R) {
                result.set_value(this->action(this->arg.get()));
            } else {
                this->action(this->arg.get());
                result.set_value();
            }
        }
    } catch(...) {
        result.set_exception(std::current_exception());
    }
}
