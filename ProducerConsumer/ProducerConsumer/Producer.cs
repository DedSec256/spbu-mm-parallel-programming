using System;
using System.Threading;
using System.Threading.Tasks;

namespace ProducerConsumer
{
    public class Producer<T>
    {
        public Producer(IQueue<T> queue, Func<T> produce, CancellationToken ct, int sleepTime = 1000)
        {
            Task.Factory.StartNew(() =>
                {
                    while (!ct.IsCancellationRequested)
                    {
                        queue.Enqueue(produce());
                        Thread.Sleep(sleepTime);
                    }
                }, ct, TaskCreationOptions.LongRunning, TaskScheduler.Default
            );
        }
    }
}
