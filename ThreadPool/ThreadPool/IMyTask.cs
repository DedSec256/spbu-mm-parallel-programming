using System;

namespace ThreadPool
{
    public interface IMyTask<out TResult>
    {
        public TResult Result { get;}
        
        public void Execute();

        public bool IsCompleted { get;}

        /// <summary>
        /// Creates a task, which is a continuation of its receiver object.
        /// Note that the task should be manually placed into a threadpool.
        /// </summary>
        /// <param name="func"></param>
        /// <typeparam name="TNewResult"></typeparam>
        /// <returns>A task which should be then placed into a threadpool.</returns>
        public IMyTask<TNewResult> ContinueWith<TNewResult>(Func<TResult, TNewResult> func);
    }
}