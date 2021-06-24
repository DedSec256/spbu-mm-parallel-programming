using System;
using System.Threading;
using System.Threading.Tasks;

namespace ProducerConsumer
{
    public class Consumer<T>
    {
        public Consumer(IQueue<T> queue, Action<T> consume, CancellationToken ct, int sleepTime = 1000)
        {
            Task.Factory.StartNew(() =>
                {
                    while (!ct.IsCancellationRequested)
                    {
                        if (queue.TryDequeue(out var result)) consume(result);
                        Thread.Sleep(sleepTime);
                    }
                }, ct, TaskCreationOptions.LongRunning, TaskScheduler.Default
            );
        }
    }
}
