using System;
using System.Collections.Generic;
using System.Threading;

namespace producer_consumer
{
    public abstract class ThreadAction<T>
    {
        protected Thread _thread;
        protected volatile TaskQueue<T> _TaskQueue;
        protected T _store;
        protected bool _cooldown;
        protected CancellationToken _token;

        public ThreadAction(TaskQueue<T> tq)
        {
            _TaskQueue = tq;
            _thread = new Thread(InternalAction);
            _thread.Start();
            _token = tq.GetToken();
            _cooldown = false;
        }

        protected abstract void Action();

        private void InternalAction()
        {
            try
            {
                Action();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Exception occurred: {ex}");
            }
        }
    }
}
