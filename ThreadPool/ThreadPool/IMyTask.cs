using System;

namespace ThreadPool
{
    public interface IMyTask<out TResult>
    {
        public bool IsCompleted { get; }
        public TResult Result { get; }
        public void Wait();
        public IMyTask<TUResult> ContinueWith<TUResult>(Func<TResult, TUResult> func);
    }
}
