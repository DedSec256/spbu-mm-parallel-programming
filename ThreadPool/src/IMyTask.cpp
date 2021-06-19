//
// Created by 19216 on 2021/5/8.
//

#include "IMyTask.h"

// 开始执行任务(Начать задачу)
void IMyTask::doWork() {
    task();
    isCompleted = true;     // 任务执行完成(Выполнение задачи завершено)
}

bool IMyTask::IsCompleted() const {
    return isCompleted;
}

// 模板类的实现不能放在.cpp文件中，只能放在.h头文件中。(Реализация класса шаблона не может быть помещена в файл .cpp, она может быть помещена только в файл заголовка .h.)
// 要想实现分离，则其实现必须放在.tpp文件中，而且在.h头文件声明完类之后再include "*.tpp"(Чтобы добиться разделения, его реализация должна быть помещена в файл .tpp, а после объявления класса в файле заголовка .h включить "* .tpp")
//template<typename F, typename... Args>
//void IMyTask::ContinueWith(F&& f, Args&&... args) {
//    f(args...);
//}
