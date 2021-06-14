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
    private Locker lockFlag;

    public Consumer(List<int> sharedData, Locker lockFlag)
    {
        this.SharedData = sharedData;
        this.lockFlag = lockFlag;
        this.Runnable = true;
        this.Thread = new Thread(() => Run());
        this.Thread.Start();
    }

    public void Stop()
    {
        this.lockFlag.Lock(); 
        this.Runnable = false;
        this.lockFlag.Release();
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
            this.lockFlag.Release();
        }
    }
}
