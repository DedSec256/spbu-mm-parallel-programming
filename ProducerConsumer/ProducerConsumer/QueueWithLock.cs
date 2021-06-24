using System.Collections.Generic;

namespace ProducerConsumer
{
    public class QueueWithLock<T> : IQueue<T>
    {
        private readonly object _locker = new();
        private readonly Queue<T> _queue = new();

        public void Enqueue(T value)
        {
            lock (_locker) _queue.Enqueue(value);
        }

        public bool TryDequeue(out T value)
        {
            lock (_locker) return _queue.TryDequeue(out value);
        }

        public bool IsEmpty
        {
            get
            {
                lock (_locker) return _queue.Count == 0;
            }
        }
    }
}
