using System;
using System.Collections.Generic;
using System.Threading;

namespace ThreadPool.MyTask
{
    public class MyTask<TResult> : IMyTask<TResult>
    {
        private readonly List<Exception> _exceptions = new List<Exception>();
        private readonly Func<TResult> _func;
        private readonly ManualResetEventSlim _ready = new ManualResetEventSlim(false, 1);
        private TResult _result;

        public bool IsCompleted => _ready.IsSet;

        public TResult Result
        {
            get
            {
                _ready.Wait();
                if (_exceptions.Count > 0)
                {
                    throw new AggregateException(_exceptions);
                }

                return _result;
            }
        }

        public MyTask(Func<TResult> func)
        {
            _func = func ?? throw new ArgumentNullException(nameof(func), "Task cannot execute null");
        }

        public void Run()
        {
            try
            {
                _result = _func.Invoke();
            }
            catch (AggregateException ae)
            {
                foreach (Exception e in ae.InnerExceptions)
                {
                    _exceptions.Add(e);
                }
            }
            catch (Exception e)
            {
                _exceptions.Add(e);
            }
            finally
            {
                _ready.Set();
            }
        }

        public IMyTask<TNewResult> ContinueWith<TNewResult>(Func<TResult, TNewResult> continuation)
        {
            return new MyTask<TNewResult>(() => continuation.Invoke(Result));
        }

        public void Dispose()
        {
            _ready?.Dispose();
        }
    }
}
