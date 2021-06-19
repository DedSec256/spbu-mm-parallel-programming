//
// Created by 19216 on 2021/5/8.
//

#include "ThreadPool.h"

ThreadPool::ThreadPool(int number): threadNum(number), stop(false) {
    for (int i = 0; i < number; ++i) {
        threadPool.emplace_back(&ThreadPool::start, this);
    }
}

void ThreadPool::Enqueue(IMyTask *task) {

    // 如果停止了，则不可再提交任务。(Если он остановится, задачи больше нельзя будет отправить.)
    if (stop.load()) return;

    unique_lock<mutex> lock(mutexTask);
    taskQueue.push(task);
    conditionVariableTask.notify_all();
}

void ThreadPool::Dispose() {
    stop.store(true);
}

void ThreadPool::start() {
    {
        unique_lock<mutex> ioLock(mutexIO);
        cout << "[info]: thread " << this_thread::get_id() << " is starting" << endl;
    }
    while (true) {
        IMyTask* task = getOneTask();       // 当没有任务时线程会阻塞在该调用函数中(Когда нет задачи, поток будет заблокирован в вызывающей функции)
        if(!task) break;
        {
            unique_lock<mutex> ioLock(mutexIO);
            cout << "[info]: thread id : " << this_thread::get_id() << " get one new task ..." << endl;
        }

        task->doWork();

        {
            unique_lock<mutex> ioLock(mutexIO);
            cout << "[info]: thread id : " << this_thread::get_id() << " has done the task." << endl;
        }

    }
}

IMyTask* ThreadPool::getOneTask() {
    if(stop.load()) {
        return nullptr;
    }
    unique_lock<mutex> lock(mutexTask);
    while (taskQueue.empty() && !stop.load()){
        // 任务队列为空并且没有停止，则条件变量调用wait方法从而阻塞当前线程，在阻塞该线程的时候lock会自动调用unlock方法释放mutexTask锁。(Если очередь задач пуста и не остановлена, переменная условия вызывает метод ожидания, чтобы заблокировать текущий поток.Когда поток заблокирован, блокировка автоматически вызовет метод разблокировки, чтобы снять блокировку mutexTask.)
        // 该线程会一直阻塞，直到条件变量conditionVariableTask调用了notify函数通知消息，这时锁lock会自动调用lock()方法从而加锁。(Поток будет блокироваться до тех пор, пока переменная условия conditionVariableTask не вызовет функцию уведомления для уведомления о сообщении. В это время блокировка блокировки автоматически вызовет метод lock () для блокировки.)
        conditionVariableTask.wait(lock);
    }
    if(stop.load()) {
        return nullptr;
    }
    IMyTask* task = taskQueue.front();
    taskQueue.pop();
    return task;
}

ThreadPool::~ThreadPool() {
    for(thread &t : threadPool) {
        t.detach();    // 让线程“自生自灭”，线程不会阻塞在这里，会让该线程 放在后台执行(Пусть поток «постоит сам за себя», поток здесь не будет заблокирован, пусть поток будет выполняться в фоновом режиме)
//        t.join();     // 等待任务结束，线程未结束前会一直阻塞在这里(Ожидая окончания задачи, поток здесь будет заблокирован до конца)
    }

}



