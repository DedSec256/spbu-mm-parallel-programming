using System;
using System.Linq;
using System.Threading;
using ProducersConsumers.Storage;
using ProducersConsumers.Workers;

namespace ProducersConsumers
{
    internal class Program
    {
        public static int Main(string[] args)
        {
            if (args.Length != 2
                || !int.TryParse(args[0], out var producersCount)
                || !int.TryParse(args[1], out var consumersCount))
            {
                Console.WriteLine("Arguments must be: <Producers count> <Consumers count>");
                return 1;
            }

            var buffer = new ProducerConsumerList<string>();
            var producers = new Worker<string>[producersCount];
            var consumers = new Worker<string>[consumersCount];
            for (var i = 0; i < producersCount; i++)
            {
                var scopeI = i;
                producers[i] = new Producer<string>(buffer, () =>
                {
                    Thread.Sleep(200);
                    return $"Data produced by Producer_{scopeI}";
                });
            }
            for (var i = 0; i < consumersCount; i++)
            {
                var scopeI = i;
                consumers[i] = new Consumer<string>(buffer, (data) =>
                {
                    Console.WriteLine($"{data} consumed by Consumer_{scopeI}");
                    Thread.Sleep(500);
                });
            }

            var workers = producers.Concat(consumers).ToArray();
            Console.WriteLine("Start producers and consumers");
            foreach (var worker in workers)
            {
                worker.Start();
            }

            Console.ReadKey();

            foreach (var worker in workers)
            {
                worker.Stop();
            }
            Console.WriteLine("All producers and consumers finished");

            return 0;
        }
    }
}
