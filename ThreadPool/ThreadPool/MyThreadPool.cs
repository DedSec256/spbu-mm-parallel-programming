using System;
using System.Collections.Concurrent;
using System.Linq;
using System.Threading;
using ThreadPool.MyTask;

namespace ThreadPool
{
    public sealed class MyThreadPool : IDisposable
    {
        private const int DefaultPoolSize = 4;

        private readonly Worker[] _workers;
        private readonly CancellationTokenSource _cts = new CancellationTokenSource();
        private readonly object _disposeLockObj = new object();
        private readonly Random _random = new Random();

        private bool _isDisposed;

        public int Size => _workers.Count(worker => worker.IsAlive);

        public MyThreadPool(int poolSize)
        {
            _workers = new Worker[poolSize];
            for (var i = 0; i < poolSize; ++i)
            {
                _workers[i] = new Worker(this, i, _cts.Token);
            }

            foreach (var worker in _workers)
            {
                worker.Start();
            }
        }

        public MyThreadPool() : this(DefaultPoolSize)
        {
        }

        public void Enqueue<TResult>(IMyTask<TResult> task)
        {
            if (task == null)
            {
                throw new ArgumentNullException(nameof(task), "Cannot run null in ThreadPool");
            }

            lock (_disposeLockObj)
            {
                if (_isDisposed)
                {
                    throw new ObjectDisposedException("Cannot add task into ThreadPool because it's disposed");
                }

                var workerId = _workers
                    .OrderBy(worker => worker.TasksCount)
                    .First()
                    .Id;
                _workers[workerId].AddTask(task.Run);
            }
        }

        public void Dispose()
        {
            lock (_disposeLockObj)
            {
                if (_isDisposed)
                {
                    throw new ObjectDisposedException("Cannot dispose ThreadPool because it's already disposed");
                }

                _isDisposed = true;
                _cts.Cancel();
                foreach (var worker in _workers)
                {
                    worker.Join();
                }

                _cts.Dispose();
            }
        }

        private bool TryStealTask(out Action task, int id)
        {
            var victimId = (id + _random.Next(_workers.Length - 1)) % _workers.Length;
            return _workers[victimId].TryStealTask(out task);
        }

        private sealed class Worker
        {
            private readonly MyThreadPool _threadPool;
            private readonly Thread _thread;
            private readonly CancellationToken _ct;
            private readonly ConcurrentQueue<Action> _waitingTasks = new ConcurrentQueue<Action>();
            private bool _isWaiting = true;

            internal int Id { get; }
            internal bool IsAlive => _thread.IsAlive;
            internal int TasksCount => _waitingTasks.Count + (_isWaiting ? 0 : 1);

            internal Worker(MyThreadPool threadPool, int id, CancellationToken ct)
            {
                _threadPool = threadPool;
                Id = id;
                _ct = ct;
                _thread = new Thread(Work);
            }

            internal void AddTask(Action task)
            {
                _waitingTasks.Enqueue(task);
            }

            internal void Start()
            {
                _thread.Start();
            }

            private void Work()
            {
                while (true)
                {
                    try
                    {
                        _ct.ThrowIfCancellationRequested();
                        if (_waitingTasks.TryDequeue(out var task) || _threadPool.TryStealTask(out task, Id))
                        {
                            _isWaiting = false;
                            task.Invoke();
                            _isWaiting = true;
                        }
                        else
                        {
                            Thread.Yield();
                        }
                    }
                    catch (OperationCanceledException)
                    {
                        break;
                    }
                }

                foreach (var task in _waitingTasks)
                {
                    task.Invoke();
                }
            }

            internal bool TryStealTask(out Action task) => _waitingTasks.TryDequeue(out task);

            internal void Join()
            {
                _thread.Join();
            }
        }
    }
}
