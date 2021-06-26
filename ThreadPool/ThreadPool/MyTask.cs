using System;
using System.Collections.Generic;

namespace ThreadPool
{
    public class MyTask<TResult> : IMyTask<TResult>
    {
        public MyTask(Func<TResult> func) => _func = func;

        public bool IsCompleted => _isCompleted;

        public TResult Result
        {
            get
            {
                Wait();
                if (_exceptions.Count > 0) throw new AggregateException(_exceptions);
                return _result;
            }
            private set => _result = value;
        }

        public void Wait()
        {
            if (_isCompleted) return;
            lock (_lockObj)
            {
                if (_isCompleted) return;
                try
                {
                    Result = _func.Invoke();
                }
                catch (AggregateException aex)
                {
                    _exceptions.AddRange(aex.InnerExceptions);
                }
                catch (Exception ex)
                {
                    _exceptions.Add(ex);
                }
                finally
                {
                    _isCompleted = true;
                }
            }
        }

        public IMyTask<TUResult> ContinueWith<TUResult>(Func<TResult, TUResult> func) =>
            new MyTask<TUResult>(() => func(Result));

        private TResult _result;
        private volatile bool _isCompleted;
        private readonly Func<TResult> _func;
        private readonly object _lockObj = new();
        private readonly List<Exception> _exceptions = new();
    }
}
