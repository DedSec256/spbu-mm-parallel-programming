using System;
using System.Collections.Generic;
using System.Threading;
using System.Text;

namespace producer_consumer
{
    public class TaskQueue<T> : IDisposable
    {
        private readonly object _queueLock;
        private readonly List<T> queue;
        private readonly CancellationTokenSource cancelTokenSource;

        public TaskQueue()
        {
            _queueLock = new object();
            queue = new List<T>();
            cancelTokenSource = new CancellationTokenSource();
        }

        public void Dispose()
        {
            cancelTokenSource.Dispose();
        }

        public CancellationToken GetToken()
        {
            return cancelTokenSource.Token;
        }

        public int Size()
        {
            return queue.Count;
        }

        public void Shutdown()
        {
            cancelTokenSource.Cancel();
        }

        public void PutTask(T item)
        {
            lock (_queueLock)
            {
                queue.Add(item);
            }
        }

        public bool TryGetTask(out T task)
        {
            lock (_queueLock)
            {
                if (queue.Count == 0)
                {
                    task = default;
                    return false;
                }

                task = queue[0];
                queue.RemoveAt(0);
                return true;
            }
        }
    }
}
