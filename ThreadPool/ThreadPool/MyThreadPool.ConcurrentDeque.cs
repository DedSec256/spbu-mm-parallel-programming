using System.Collections.Generic;
using System.Linq;

namespace ThreadPool
{
    public partial class MyThreadPool
    {
        private class DequeWithLock<T>
        {
            private readonly object _lock = new();
            private readonly LinkedList<T> _deque;

            public DequeWithLock() => _deque = new LinkedList<T>();

            public void Push(T value)
            {
                lock (_lock) _deque.AddLast(value);
            }

            public bool TryPopBottom(out T value)
            {
                lock (_lock)
                {
                    value = default;
                    if (_deque.Count == 0) return false;

                    value = _deque.First();
                    _deque.RemoveFirst();

                    return true;
                }
            }

            public bool TryPopTop(out T value)
            {
                lock (_lock)
                {
                    value = default;
                    if (_deque.Count == 0) return false;

                    value = _deque.Last();
                    _deque.RemoveLast();

                    return true;
                }
            }
        }
    }
}
