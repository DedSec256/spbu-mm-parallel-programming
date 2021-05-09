using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

class Consumer
{
    Thread Thread;
    List<int> SharedData;
    bool Runnable = false;
    private const int pause = 2000;

    public Consumer(List<int> sharedData, Locker lockFlag)
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
                if (SharedData.Count == 0)
                {
                    Console.WriteLine("Consumer: list is empty");
                }
                else
                {
                    int value = SharedData.Last();
                    SharedData.Remove(value);
                    Console.WriteLine("Consumer: removed " + value + " from list");
                }
            }
            lockFlag.Release();
            Thread.Sleep(pause);
        }
    }
}
