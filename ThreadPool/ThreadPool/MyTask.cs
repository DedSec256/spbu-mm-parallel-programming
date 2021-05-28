using System;
using System.Collections.Generic;
using System.Threading;

namespace ThreadPool
{
    public class MyTask<TResult> : IMyTask<TResult>
    {

        public TResult Result
        {
            get
            {
                _ready.WaitOne();
                if (_exceptions.Count > 0)
                {
                    throw new AggregateException(_exceptions);
                }

                return _result;
            }
            private set => _result = value;
        }

        private readonly Func<TResult> _func;
        private TResult _result;
        private readonly List<Exception> _exceptions = new List<Exception>();
        private readonly ManualResetEvent _ready = new ManualResetEvent(false);

        public MyTask(Func<TResult> func)
        {
            _func = func;
        }

        public void Execute()
        {
            try
            {
                Result = _func.Invoke();
            }
            catch (AggregateException aex)
            {
                foreach (var ex in aex.InnerExceptions)
                {
                    _exceptions.Add(ex);
                }
            }
            catch (Exception ex)
            {
                _exceptions.Add(ex);
            }
            finally
            {
                _ready.Set();
            }
            
        }

        public bool IsCompleted => _ready.WaitOne(0);

        public IMyTask<TNewResult> ContinueWith<TNewResult>(Func<TResult, TNewResult> func)
        {
            return new MyTask<TNewResult>(() => func(this.Result));
        }

        //hope this is ok :)
        ~MyTask()
        {
            _ready.Dispose();
        }
        
    }
}