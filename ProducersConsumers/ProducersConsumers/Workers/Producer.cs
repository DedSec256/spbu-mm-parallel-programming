using System;
using ProducersConsumers.Storage;

namespace ProducersConsumers.Workers
{
    public class Producer<T> : Worker<T>
    {
        private readonly Func<T> _produce;

        public Producer(IProducerConsumerBuffer<T> storage, Func<T> produce) : base(storage)
        {
            _produce = produce;
        }

        protected override void Work()
        {
            Storage.Add(_produce.Invoke());
        }
    }
}
