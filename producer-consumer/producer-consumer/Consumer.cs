using System;
using System.Collections.Generic;
using System.Threading;

namespace producer_consumer
{
    public class Consumer<T> : ThreadAction<T>
    {
        public Consumer(TaskQueue<T> tq)
            : base(tq)
        {
        }

        protected override void Action()
        {
            while (!_token.IsCancellationRequested)
            {
                if (!_cooldown && _TaskQueue.TryGetTask(out _store))
                {
                    _cooldown = true;
                    Console.WriteLine("Consumed by {0}, " + _store, Thread.GetCurrentProcessorId());
                }
                else
                {
                    Thread.Sleep(2000);
                    _cooldown = false;
                }
            }
        }
    }
}
