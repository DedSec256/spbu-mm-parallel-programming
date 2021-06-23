using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

class Consumer
{
    private Thread workerThread;
    private List<int> sharedData;
    private volatile bool runnable = false;
    private const int pause = 2000;
    private Locker lockFlag;

    public Consumer(List<int> sharedData, Locker lockFlag)
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
                if (sharedData.Count == 0)
                {
                    Console.WriteLine("Consumer: list is empty");
                }
                else
                {
                    int value = sharedData.Last();
                    sharedData.Remove(value);
                    Console.WriteLine("Consumer: removed " + value + " from list");
                }
            }
            this.lockFlag.Release();
        }
    }
}
