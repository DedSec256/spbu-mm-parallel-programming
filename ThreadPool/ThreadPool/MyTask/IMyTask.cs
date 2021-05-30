using System;

namespace ThreadPool.MyTask
{
    public interface IMyTask<out TResult> : IDisposable
    {
        /// <returns>True if task was finished</returns>
        bool IsCompleted { get; }

        /// <summary>
        /// Get task result.
        /// If task fails, <see cref="AggregateException"/> will thrown.
        /// If result is not ready
        /// current thread is blocked until task
        /// task completes or fails. 
        /// </summary>
        TResult Result { get; }

        /// <summary>
        /// Execute task synchronously. To execute it in parallel
        /// enqueue it into thread pool using <see cref="MyThreadPool.Enqueue{TResult}"/> 
        /// </summary>
        void Run();

        /// <summary>
        /// Be careful when enqueuing a continuation into
        /// thread pool before the base task, this can lead to deadlock!
        /// </summary>
        /// <param name="continuation"></param>
        /// <typeparam name="TNewResult"></typeparam>
        /// <returns>
        /// New task 
        /// </returns>
        IMyTask<TNewResult> ContinueWith<TNewResult>(Func<TResult, TNewResult> continuation);
    }
}
