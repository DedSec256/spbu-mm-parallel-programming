using System;
using System.Threading;

namespace ProducerConsumer
{
    public class Consumer<T> : Worker<T>
    {

        private Func<T, object> _consume;
        private CancellationToken _ct;
        private Thread _consumerThread;
        
        public Consumer(Buffer<T> buffer, Func<T,object> consume, CancellationToken ct,int sleepTime = 1000) : base(buffer)
        {
            _consume = consume;
            _ct = ct;

            _consumerThread = new Thread(() =>
                {
                    while (!_ct.IsCancellationRequested)
                    {
                        Maybe<T> result = _buffer.Pop();
                        
                        result.Match<object>((T _result) =>
                        {
                            _consume(_result);
                            return null;
                        }, () => null);
                        
                        Thread.Sleep(sleepTime);
                    }
                }
            );
            _consumerThread.Start();
        }

        public override void Wait()
        {
            _consumerThread.Join();
        }
    }
}