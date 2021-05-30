namespace ProducersConsumers.Storage
{
    public interface IProducerConsumerBuffer<T>
    {
        void Add(T item);

        bool TryTake(out T item);
    }
}
