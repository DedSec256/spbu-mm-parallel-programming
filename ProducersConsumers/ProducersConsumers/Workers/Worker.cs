using System;
using System.Threading;
using ProducersConsumers.Storage;

namespace ProducersConsumers.Workers
{
    public abstract class Worker<T>
    {
        private readonly Thread _thread;
        private volatile int _isActive = 0;

        protected IProducerConsumerBuffer<T> Storage { get; }

        protected abstract void Work();

        protected Worker(IProducerConsumerBuffer<T> storage)
        {
            Storage = storage;
            _thread = new Thread(ThreadWork);
        }

        public void Start()
        {
            if (Interlocked.CompareExchange(ref _isActive, 1, 0) == 0)
            {
                _thread.Start();
            }
        }

        private void ThreadWork()
        {
            while (_isActive > 0)
            {
                Work();
            }
        }

        public void Stop()
        {
            if (Interlocked.Exchange(ref _isActive, 0) == 1)
            {
                _thread.Join();
            }
        }
    }
}
