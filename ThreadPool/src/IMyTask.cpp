//
// Created by 19216 on 2021/5/8.
//

#include "IMyTask.h"

// 开始执行任务
void IMyTask::doWork() {
    task();
    isCompleted = true;     // 任务执行完成
}

bool IMyTask::IsCompleted() const {
    return isCompleted;
}

// 模板类的实现不能放在.cpp文件中，只能放在.h头文件中。
// 要想实现分离，则其实现必须放在.tpp文件中，而且在.h头文件声明完类之后再include "*.tpp"
//template<typename F, typename... Args>
//void IMyTask::ContinueWith(F&& f, Args&&... args) {
//    f(args...);
//}
