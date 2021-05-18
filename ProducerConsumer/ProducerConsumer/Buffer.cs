using System;
using System.Collections.Generic;
using System.Threading;

namespace ProducerConsumer
{
    public class Buffer<T> : IBuffer<T>
    {
        private object _locker;
        private List<T> _buffer;
        private CancellationTokenSource _cts;
            
        public Buffer(CancellationTokenSource cts)
        {
            _locker = new object();
            _buffer = new List<T>();
            _cts = cts;
        }

        public void Cancel()
        {
            _cts.Cancel();
        }
        
        public void Dispose()
        {
            _buffer.Clear();
        }

        public void Push(T value)
        {
            lock (_locker)
            {
                _buffer.Add(value);
            }
        }

        public Maybe<T> Pop()
        {
            lock (_locker)
            {
                if (_buffer.Count == 0) return new Maybe<T>();
                
                var result = _buffer[0];
                _buffer.RemoveAt(0);
                return new Maybe<T>(result);

            }
        }

        public bool isEmpty()
        {
            lock (_locker)
            {
                return _buffer.Count == 0;
            }
        }
    }
}