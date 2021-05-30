using System;
using ProducersConsumers.Storage;

namespace ProducersConsumers.Workers
{
    public class Consumer<T> : Worker<T>
    {
        private readonly Action<T> _consume;

        public Consumer(IProducerConsumerBuffer<T> storage, Action<T> consume) : base(storage)
        {
            _consume = consume;
        }

        protected override void Work()
        {
            if (Storage.TryTake(out var data))
            {
                _consume.Invoke(data);
            }
        }
    }
}
