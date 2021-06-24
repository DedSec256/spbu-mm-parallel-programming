namespace ProducerConsumer
{
    public interface IQueue<T>
    {
        bool IsEmpty { get; }
        void Enqueue(T value);
        bool TryDequeue(out T value);
    }
}
