using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;


namespace ProducerConsumer
{
    class Program
    {
        private const int prodNum = 5;
        private const int consNum = 5;

        static void Main(string[] args)
        {
            // shared data
            List<int> sharedData = new List<int>();

            Locker lockFlag = new Locker();

            List<Producer> prodList = new List<Producer>();
            List<Consumer> consList = new List<Consumer>();

            // starting producers
            for (int i = 0; i < prodNum; ++i)
            {
                Producer pr = new Producer(sharedData, lockFlag);
                prodList.Add(pr);
            }

            // starting consumers
            for (int i = 0; i < consNum; ++i)
            {
                Consumer co = new Consumer(sharedData, lockFlag);
                consList.Add(co);
            }

            // "kill" signal
            Console.ReadKey();
            Console.WriteLine("Stopping all threads...");

            // stopping and clearing list
            for (int i = 0; i < prodNum; ++i)
                prodList[i].Stop();

            for (int i = 0; i < consNum; ++i)
                consList[i].Stop();
     
            // final state of sharedDate
            sharedData.ForEach(i => Console.Write(i + " "));
            Console.ReadKey();
        }
    }
}
