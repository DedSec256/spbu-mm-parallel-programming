using System;
using System.Collections.Generic;
using System.Threading;

namespace producer_consumer
{
    public class Producer<T> : ThreadAction<T>
    {
        private readonly Func<T> _produce;

        public Producer(TaskQueue<T> tq)
            : this(tq, () => default(T))
        {
        }

        public Producer(TaskQueue<T> tq, Func<T> func)
            : base(tq)
        {
            _produce = func;
        }

        protected override void Action()
        {
            while (!_token.IsCancellationRequested)
            {
                if (!_cooldown)
                {
                    _store = _produce();
                    _cooldown = true;
                    _TaskQueue.PutTask(_store);
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
