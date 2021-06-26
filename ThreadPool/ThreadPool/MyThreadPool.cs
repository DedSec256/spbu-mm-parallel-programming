using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace ThreadPool
{
    public partial class MyThreadPool : IDisposable
    {
        public MyThreadPool(int numberOfThreads)
        {
            for (var i = 0; i < numberOfThreads; i++)
            {
                var worker = new Worker(this);
                _workers.Add(worker);
                _workerQueues.Add(worker.WorkerId, new DequeWithLock<Action>());
            }

            foreach (var worker in _workers) worker.Start();
        }

        public int NumberOfWorkers => _workers.Count(worker => worker.IsAlive);

        public IMyTask<TResult> Enqueue<TResult>(IMyTask<TResult> task)
        {
            if (_cts.IsCancellationRequested) throw new OperationCanceledException("MyThreadPool disposed");

            var currentThreadId = Environment.CurrentManagedThreadId;

            if (_workerQueues.ContainsKey(currentThreadId))
            {
                _workerQueues[currentThreadId].Push(task.Wait);
            }
            else
            {
                _threadPoolTaskQueue.Enqueue(task.Wait);
            }

            return task;
        }

        public IMyTask<TResult> Enqueue<TResult>(Func<TResult> func) => Enqueue(new MyTask<TResult>(func));

        /// Workers run till all queues become empty.
        public void Dispose()
        {
            lock (_disposeLockObj)
            {
                if (_isDisposed) return;

                _cts.Cancel();

                foreach (var worker in _workers) worker.Join();

                _cts?.Dispose();
                _isDisposed = true;
            }
        }

        private DequeWithLock<Action> ChooseWorker() =>
            _workerQueues.ElementAt(_rand.Next(0, _workerQueues.Count)).Value;

        private bool RunNextTask()
        {
            var workerId = Environment.CurrentManagedThreadId;

            if (_workerQueues[workerId].TryPopBottom(out var currentWorkerAction))
            {
                currentWorkerAction();
                return true;
            }

            if (_threadPoolTaskQueue.TryDequeue(out var threadPoolAction))
            {
                threadPoolAction();
                return true;
            }

            var workerToSteal = ChooseWorker();
            if (workerToSteal.TryPopTop(out var anotherWorkerAction))
            {
                anotherWorkerAction();
                return true;
            }

            return !_cts.IsCancellationRequested;
        }

        private bool _isDisposed;
        private readonly Random _rand = new();
        private readonly List<Worker> _workers = new();
        private readonly object _disposeLockObj = new();
        private readonly CancellationTokenSource _cts = new();
        private readonly ConcurrentQueue<Action> _threadPoolTaskQueue = new();
        private readonly Dictionary<int, DequeWithLock<Action>> _workerQueues = new();
    }
}
