//
// Created by 19216 on 2021/5/8.
//

#ifndef THREADPOOL_IMYTASK_H
#define THREADPOOL_IMYTASK_H

#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include <iostream>
#include <future>

using namespace std;

class IMyTask {

    using Task = function<void()>;

private:
    bool isCompleted;
    Task task;
    string taskName;

public:
    // 模板函数的实现必须放在头文件中，不能放在cpp文件中，要想分离可以放在tpp文件中(Реализация функции шаблона должна быть помещена в файл заголовка, а не в файл cpp, если вы хотите разделить, ее можно поместить в файл tpp)
    template<typename Func, typename... Args>
    auto ContinueWith(Func&& f, Args&&... args) -> future<decltype(f(args...))> {
        isCompleted = false;
        // 函数返回值类型(Тип возвращаемого значения функции)
        using resType = decltype(f(args...));
        // packaged_task<> 与 function<> 类似，都是一个可调用对象；packaged_task<int()>, function<int()>(packaged_task <> похожа на function <>, оба являются вызываемыми объектами; packaged_task <int ()>, function <int ()>)
        // 不过packaged_task<>是异步调用对象，还保存有执行的返回信息在future<>中(Однако packaged_task <> является объектом асинхронного вызова, и возвращаемая информация о выполнении также сохраняется в будущем <>)
        // 利用传入的函数f与该函数的参数args构造新的可调用对象，通过bind函数使其生成一个无参数的可调用对象(Создайте новый вызываемый объект, используя входящую функцию f и аргументы параметра функции, и сгенерируйте вызываемый объект без параметров с помощью функции привязки)
        auto callable = make_shared<packaged_task<resType()> >(bind(f,args...));

        // 利用lambda表达式构造新的可调用函数，使其满足通用的任务可调用函数Task：function<void()>(Используйте лямбда-выражения для создания новой вызываемой функции для удовлетворения общей вызываемой функции задачи Task: function <void ()>)
        // 无参数并且返回值为void的可调用对象，调用时直接task();即可(Вызываемый объект без параметров и возвращаемого значения void, непосредственно task (); может использоваться при вызове)
        task = [callable]() {
            (*callable)();
        };
        // f(args...)调用的返回值，异步获取返回值; result类型为future<resType>(Возвращаемое значение вызова f (args ...) получается асинхронно; тип результата - future <resType>)
        auto result = callable->get_future();
        return result;
    }

    bool IsCompleted() const;
    void doWork();
};

#endif //THREADPOOL_IMYTASK_H