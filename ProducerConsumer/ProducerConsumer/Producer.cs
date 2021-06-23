using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

namespace ProducerConsumer
{

    class Producer
    {
        private Thread workerThread;
        private List<int> sharedData;
        volatile bool runnable = false;
        private const int pause = 1000;
        private Locker lockFlag;

        public Producer(List<int> sharedData, Locker lockFlag)
        {
            this.sharedData = sharedData;
            this.lockFlag = lockFlag;
            this.runnable = true;
            this.workerThread = new Thread(() => Run());
            this.workerThread.Start();
        }

        public void Stop()
        {
            this.runnable = false;
            this.workerThread.Join();
        }

        public void Run()
        {
            while (this.runnable)
            {
                this.lockFlag.Lock();
                // Stil running ? 
                if (this.runnable)
                {
                    Thread.Sleep(pause);
                    // push some value into shared data
                    this.sharedData.Add(24);
                    Console.WriteLine("Producer: added value 24");
                }
                this.lockFlag.Release();
            }

        }

    }
}
