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
        bool Runnable = false;
        private const int pause = 1000;

        public Producer(List<int> sharedData, Locker lockFlag)
        {
            this.SharedData = sharedData;
            this.Runnable = true;
            this.Thread = new Thread(() => Run(lockFlag));
            this.Thread.Start();
        }

        public void Stop()
        {
            this.Runnable = false;
            this.Thread.Join();
        }

        public void Run(Locker lockFlag)
        {
            while (this.Runnable)
            {
                lockFlag.Lock();
                // Stil running ? 
                if (this.Runnable)
                {
                    // push some value into shared data
                    this.SharedData.Add(24);
                    Console.WriteLine("Producer: added value 24");
                }
                lockFlag.Release();
                Thread.Sleep(pause);
            }

        }

    }

}
