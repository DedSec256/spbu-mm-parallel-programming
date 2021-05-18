using System;
using System.Collections.Generic;
using System.Threading;

namespace ProducerConsumer
{
    public class Producer<T> : Worker<T>
    {
        public Producer(Buffer<T> buffer, Func<T> produce, CancellationToken ct, int sleepTime = 1000) : base(buffer)
        {
            _produce = produce;

            _producerThread= new Thread(() =>
                {
                    while (!ct.IsCancellationRequested)
                    {
                        
                        T result = _produce();
                        
                        _buffer.Push(result);
                        Thread.Sleep(sleepTime);
                    }
                }
            );
            _producerThread.Start();
        }

        public override void Wait()
        {
            _producerThread.Join();
        }
        
        private Func<T> _produce;
        private Thread _producerThread;

    }
}