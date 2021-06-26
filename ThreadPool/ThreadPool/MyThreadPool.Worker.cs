using System.Threading;

namespace ThreadPool
{
    public partial class MyThreadPool
    {
        private class Worker
        {
            public Worker(MyThreadPool tp)
            {
                _thread = new Thread(() =>
                {
                    while (tp.RunNextTask()) {}
                });
            }

            public int WorkerId => _thread.ManagedThreadId;
            public bool IsAlive => _thread.IsAlive;
            public void Start() => _thread.Start();
            public void Join() => _thread.Join();

            private readonly Thread _thread;
        }
    }
}
