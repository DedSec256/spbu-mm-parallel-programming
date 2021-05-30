using System.Collections.Generic;

namespace ProducersConsumers.Storage
{
    public class ProducerConsumerList<T> : IProducerConsumerBuffer<T>
    {
        private readonly object _lockObj = new object();
        private readonly List<T> _items = new List<T>();

        public void Add(T item)
        {
            lock (_lockObj)
            {
                _items.Add(item);
            }
        }

        public bool TryTake(out T item)
        {
            lock (_lockObj)
            {
                if (_items.Count > 0)
                {
                    item = _items[_items.Count - 1];
                    _items.RemoveAt(_items.Count - 1);
                    return true;
                }
                item = default;
                return false;
            }
        }
    }
}
