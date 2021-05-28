using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace ThreadPool
{
    public class ThreadPool : IDisposable
    {

        private class ConcurrentDeque<T> : IDEQueue<T>
        {
            private readonly object _lock = new object();
            private readonly List<T> _deque;

            public ConcurrentDeque()
            {
                _deque = new List<T>();
            }
            
            public void Push(T value)
            {
                lock (_lock)
                {
                    _deque.Add(value);
                }
                
            }

            public Maybe<T> PopBottom()
            {
                lock (_lock)
                {
                    if (_deque.Count == 0) return new Maybe<T>();
                    
                    var result = _deque.First();
                    _deque.RemoveAt(0);
                        
                    return new Maybe<T>(result);

                }
            }

            public Maybe<T> PopTop()
            {
                lock (_lock)
                {
                    if (_deque.Count == 0) return new Maybe<T>();
                    var result = _deque.Last();
                    _deque.RemoveAt(_deque.Count - 1);
                        
                    return new Maybe<T>(result);

                }
            }

            public bool IsEmpty()
            {
                lock (_lock)
                {
                    return _deque.Count == 0;
                }
            }
        }
        private class Worker
        {
            private readonly Thread _thread;
            public int WorkerId => _thread.ManagedThreadId;

            public bool IsAlive => _thread.IsAlive;
            

            public Worker(ThreadPool tp, Dictionary<int,bool> workerFinished)
            {
                _thread = new Thread(() =>
                {
                    while (true)
                    {
                        tp.RunPendingTask();
                        
                        // then all deques are empty and we are done
                        if(workerFinished[WorkerId]) break;
                    }
                    
                });
                
            }


            public void Start()
            {
                _thread.Start();
            }

            public void Join()
            {
                _thread.Join();
            }
            
        }
        
        private readonly ConcurrentQueue<Action> _taskQueue;

        private readonly Dictionary<int, IDEQueue<Action>> _workerQueues;
        private readonly List<Worker> _workers;
        private readonly HashSet<int> _workersIds;
        private readonly Dictionary<int,bool> _workersFinished;
        
        private readonly CancellationTokenSource _cts;
        
        
        
        private bool _isDisposed = false;
        private readonly object _disposeLockObj = new object();
        
        public int NumberOfWorkers => _workers.Count(worker => worker.IsAlive);

        /// <summary>
        /// Work-stealing threadpool
        /// </summary>
        /// <param name="numberOfThreads">Number of threads the pool owns</param>
        public ThreadPool(int numberOfThreads)
        {
            _taskQueue = new ConcurrentQueue<Action>();
            _workerQueues = new Dictionary<int, IDEQueue<Action>>();
            
            _cts = new CancellationTokenSource();
            _workers = new List<Worker>();
            _workersIds = new HashSet<int>();
            _workersFinished = new Dictionary<int, bool>();

            for (int i = 0; i < numberOfThreads; i++)
            {
                var worker = new Worker(this,_workersFinished);
                _workerQueues.Add(worker.WorkerId, new ConcurrentDeque<Action>());
                _workersFinished.Add(worker.WorkerId, false);
                
                _workers.Add(worker);
                _workersIds.Add(worker.WorkerId);
            }

            foreach (var worker in _workers)
            {
                worker.Start();
            }
        }

        /// <summary>
        /// Enqueues a <see cref="Func{TResult}"/> object into a threadpool. Note that a task could spawn new tasks and are
        /// allowed to do busy-waiting making <see cref="RunPendingTask"/>. If too much <see cref="RunPendingTask"/> is invoked
        /// <see cref="StackOverflowException"/> could be thrown
        /// </summary>
        /// <param name="func"></param>
        /// <typeparam name="TResult"></typeparam>
        /// <returns>A task object which could be waited for result</returns>
        /// <exception cref="OperationCanceledException">If threadpool has been stopped</exception>
        /// <exception cref="StackOverflowException"> If recursion depth is exceeded</exception>
        public IMyTask<TResult> Enqueue<TResult>(Func<TResult> func)
        {
            if (_cts.IsCancellationRequested) throw new OperationCanceledException("Thread pool has been shut down");
            
            IMyTask<TResult> task = new MyTask<TResult>(func);

            var currentThreadId = Thread.CurrentThread.ManagedThreadId;
            if (_workersIds.Contains(currentThreadId))
            {
                _workerQueues[currentThreadId].Push(() => task.Execute());
            }
            else
            {
                _taskQueue.Enqueue(() => task.Execute());    
            }
            
            return task;
        }

        /// <summary>
        /// Enqueues a <see cref="IMyTask{TResult}"/> object into a threadpool. Note that a task could spawn new tasks and are
        /// allowed to do busy-waiting making <see cref="RunPendingTask"/>. If too much <see cref="RunPendingTask"/> is invoked
        /// <see cref="StackOverflowException"/> could be thrown
        /// </summary>
        /// <param name="task"></param>
        /// <typeparam name="TResult"></typeparam>
        /// <returns></returns>
        /// <exception cref="OperationCanceledException"></exception>
        public IMyTask<TResult> Enqueue<TResult>(IMyTask<TResult> task)
        {
            if (_cts.IsCancellationRequested) throw new OperationCanceledException("Thread pool has been shut down");

            var currentThreadId = Thread.CurrentThread.ManagedThreadId;
            
            if (_workersIds.Contains(currentThreadId))
            {
                _workerQueues[currentThreadId].Push(task.Execute);
            }
            else
            {
                _taskQueue.Enqueue(task.Execute);    
            }
            
            return task;
        }

        /// <summary>
        /// Run a cycle of worker routine namely try pop task from queue or try to steal
        /// </summary>
        /// <exception cref="ApplicationException">If the method is called from non-pooled thread</exception>
        public void RunPendingTask()
        {
            var workerId = Thread.CurrentThread.ManagedThreadId;
            
            if (!_workersIds.Contains(workerId)) throw new ApplicationException("Cannot invoke this method from non-pooled thread");
            
            _workerQueues[workerId].PopBottom().Match<object>((action) =>
                        {
                            action();
                            return null;
                        }, () =>
                        {
                            //get job from main queue
                            if (_taskQueue.TryDequeue(out var action))
                            {
                                action();
                                return null;
                            }
                            else //try to steal
                            {
                                var rand = new Random();
                                try
                                {
                                    var victim = _workerQueues.ElementAt(rand.Next(0, _workerQueues.Count)).Value;
                                    victim.PopTop().Match<object>((action) =>
                                        {
                                            action();
                                            return null;
                                        },
                                        () => //if we are here and ct is Cancelled we need to wait until all deques become empty
                                        {
                                            
                                            if (!_cts.IsCancellationRequested)
                                            {
                                                Thread.Yield();
                                                return null;
                                            }
                                            
                                            foreach (var deque in _workerQueues.Values)
                                            {
                                            
                                                while (!deque.IsEmpty())
                                                {
                                                    deque.PopTop().Match<object>((action) =>
                                                    {
                                                        action();
                                                        return null;
                                                    }, () => null);    
                                                }
                                                    
                                            }
                                            
                                            _workersFinished[workerId] = true;
                                            return null;

                                        });
                                    
                                }
                                catch(ArgumentOutOfRangeException) {}
                            }

                            
                            return null;
                        });
            
            
        }

        /// <summary>
        /// Stops the execution of the threadpool. Workers run till all queues become empty.
        /// </summary>
        /// <exception cref="ObjectDisposedException">If dispose after dispose is called</exception>
        public void Dispose()
        {
            lock (_disposeLockObj)
            {

                if (_isDisposed) return;

                _cts.Cancel();

                _isDisposed = true;

                foreach (var worker in _workers)
                {
                    worker.Join();
                }
                
                _cts?.Dispose();
            }
            
        }
    }
}