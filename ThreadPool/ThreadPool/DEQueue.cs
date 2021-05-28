namespace ThreadPool
{
    public interface IDEQueue<T>
    {
        void Push(T value);
        
        Maybe<T> PopBottom();
        
        Maybe<T> PopTop();
        
        bool IsEmpty();
    }
}