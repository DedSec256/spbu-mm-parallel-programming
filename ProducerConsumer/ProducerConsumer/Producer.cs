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
        Thread Thread;
        List<int> SharedData;
        volatile bool Runnable = false;
        private const int pause = 1000;
        private Locker lockFlag;

        public Producer(List<int> sharedData, Locker lockFlag)
        {
            this.SharedData = sharedData;
            this.lockFlag = lockFlag;
            this.Runnable = true;
            this.Thread = new Thread(() => Run());
            this.Thread.Start();
        }

        public void Stop()
        {
            this.Runnable = false;
            this.Thread.Join();
        }

        public void Run()
        {
            while (this.Runnable)
            {
                this.lockFlag.Lock();
                // Stil running ? 
                if (this.Runnable)
                {
                    Thread.Sleep(pause);
                    // push some value into shared data
                    this.SharedData.Add(24);
                    Console.WriteLine("Producer: added value 24");
                }
                this.lockFlag.Release();
            }

        }

    }

}
