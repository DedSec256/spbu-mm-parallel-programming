#include <iostream>
#include <string>
#include "src/IMyTask.h"
#include "src/ThreadPool.h"
using namespace std;

// 模拟任务1（Simulation task 1）
string Task1(string s) {
    this_thread::sleep_for(chrono::microseconds(1000));
    return s+s;
}

// 模拟任务2（Simulation task 2）
int Task2(int a, int n) {
    this_thread::sleep_for(chrono::microseconds(2000));
    if(a == 0) return 0;
    if(n == 0) return 1;
    if(n < 0) {
        a = 1/a;
        n = -n;
    }
    int res = 1;
    for(int i = 0; i < n; ++i) {
        res *= a;
    }
    return res;
}

// 模拟任务3（Simulation task 3）
int Task3(int a) {
    this_thread::sleep_for(chrono::microseconds(500));
    return a * a;
}

// 模拟任务4（Simulation task 4）
int Task4(int a, int b) {
    this_thread::sleep_for(chrono::microseconds(500));
    return a + b;
}

// 向线程池中只添加一个任务（Add only one task to the thread pool）
void test_one_task(int thread_number) {
    try {
        cout << "############## test one task ############" << endl;

        // 新建线程池对象（New thread pool object）
        shared_ptr<IDisposable> disposable(new ThreadPool(thread_number));

        // 新建任务对象（New task object）
        shared_ptr<IMyTask> task(new IMyTask());

        // 添加任务（Add task）
        auto result = task->ContinueWith(Task1, "Hello world! ");
        disposable->Enqueue(task.get());

        // 得到任务执行的结果（Get the result of the task execution）
        cout << "[info]: get task result : " << result.get() << endl;
        disposable->Dispose();
        cout << "############ end test one task ###########\n" << endl;

    }catch (exception &e) {
        cout << "[error]: catch one error : " << e.what() << endl;
    }
}

// 向线程池中添加多个任务（Add multiple tasks to the thread pool）
void test_multi_task(int thread_number) {
    try {
        cout << "############## test multi task ############" << endl;

        // 新建线程池对象（New thread pool object）
        shared_ptr<IDisposable> disposable(new ThreadPool(thread_number));

        // 新建任务对象（New task object）
        shared_ptr<IMyTask> task1(new IMyTask());
        auto result1 = task1->ContinueWith(Task1, "Hello world! ");
        disposable->Enqueue(task1.get());

        shared_ptr<IMyTask> task2(new IMyTask());
        auto result2 = task2->ContinueWith(Task2, 2, 20);
        disposable->Enqueue(task2.get());

        shared_ptr<IMyTask> task3(new IMyTask());
        auto result3 = task3->ContinueWith(Task3, 199);
        disposable->Enqueue(task3.get());

        shared_ptr<IMyTask> task4(new IMyTask());
        auto result4 = task4->ContinueWith(Task4, 9999, 99999);
        disposable->Enqueue(task4.get());

        cout << "[info]: get Task1 result : " << result1.get() << endl;
        cout << "[info]: get Task2 result : " << result2.get() << endl;
        cout << "[info]: get Task3 result : " << result3.get() << endl;
        cout << "[info]: get Task4 result : " << result4.get() << endl;

        disposable->Dispose();

        cout << "############ end test multi task ###########\n" << endl;

    }catch (exception &e) {
        cout << "[error]: catch one error : " << e.what() << endl;
    }
}

// 测试ContinueWith（Test ContinueWith）
void test_task_ContinueWith(int thread_number) {
    try {
        cout << "############## test ContinueWith ############" << endl;

        // 新建线程池对象（New thread pool object）
        shared_ptr<IDisposable> disposable(new ThreadPool(thread_number));

        // 新建任务对象（New task object）
        shared_ptr<IMyTask> task(new IMyTask());

        auto result1 = task->ContinueWith(Task1, "Hello world! ");
        disposable->Enqueue(task.get());
        // 必须在对result调用get()之后才可以对task继续调用ContinueWith设置新的任务（ must call get() on the result before you can continue to call ContinueWith on the task to set a new task）
        // 因为只有在调用get()之后才可以确保上一个任务已经执行完毕，如果上一个任务没有执行完毕，则这里函数会阻塞（Because only after calling get() can it be ensured that the previous task has been executed. If the previous task has not been executed, this function will block）
        cout << "[info]: get Task1 result : " << result1.get() << endl;

        auto result2 = task->ContinueWith(Task2, 2, 20);
        disposable->Enqueue(task.get());
        cout << "[info]: get Task2 result : " << result2.get() << endl;

        auto result3 = task->ContinueWith(Task3, 199);
        disposable->Enqueue(task.get());
        cout << "[info]: get Task3 result : " << result3.get() << endl;

        auto result4 = task->ContinueWith(Task4, 9999, 99999);
        disposable->Enqueue(task.get());
        cout << "[info]: get Task4 result : " << result4.get() << endl;

        disposable->Dispose();

        cout << "############ end test ContinueWith ###########\n" << endl;

    }catch (exception &e) {
        cout << "[error]: catch one error : " << e.what() << endl;
    }
}

int main(int argc, char* argv[]) {

    if(argc > 2) {
        cout << "Usage: " << argv[0] << " [thread number], default thread number is 5" << endl;
        return 0;
    }

    int threadNumber = 5;
    if(argc == 2) {
        threadNumber = stoi(argv[1]);
    }
    cout << "[info]: threadPool's thread number is : " << threadNumber << endl;
    cout << "[info]: main function thread id is : " << this_thread::get_id() << endl;

    test_one_task(threadNumber);
    test_multi_task(threadNumber);
    test_task_ContinueWith(threadNumber);

    system("pause");

    return 0;
}
