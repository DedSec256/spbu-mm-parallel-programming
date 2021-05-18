using System;

namespace ProducerConsumer
{
    public interface IBuffer<T> : IDisposable
    {
        void Push(T value);
        Maybe<T> Pop();

        bool isEmpty();
    }
}