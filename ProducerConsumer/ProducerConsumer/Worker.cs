using System.Threading;

namespace ProducerConsumer
{
    public abstract class Worker<T>
    {
        protected Buffer<T> _buffer;

        protected Worker(Buffer<T> buffer)
        {
            _buffer = buffer;
        }

        public abstract void Wait();
    }
}