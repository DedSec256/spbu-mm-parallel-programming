using System;
using System.Collections.Generic;
using System.Threading;

namespace producer_consumer
{
    class Program
    {
        private const int pCount = 5;
        private const int cCount = 3;

        private static string func ()
        {
            return String.Format("Produced by {0}", Thread.GetCurrentProcessorId());
        }

        private static void Run()
        {
            using (var taskQueue = new TaskQueue<string>())
            {
                Console.WriteLine("Starting producers...");
                var producers = new List<Producer<string>>();
                for (int i = 0; i < pCount; ++i)
                {
                    producers.Add(new Producer<string>(taskQueue, func));
                }

                Console.WriteLine("Starting consumers...");
                var consumers = new List<Consumer<string>>();
                for (int i = 0; i < cCount; ++i)
                {
                    consumers.Add(new Consumer<string>(taskQueue));
                }

                Console.ReadKey();

                taskQueue.Shutdown();
            }
        }

        static int Main(string[] args)
        {
            try
            {
                Run();
                return 0;
            }
            catch (Exception ex)
            {
                string exceptionMessage = $"Exception occurred in {nameof(Main)} method. " +
                                          $"{Environment.NewLine}{ex}";
                Console.WriteLine(exceptionMessage);

                return -1;
            }
            finally
            {
                Console.WriteLine("Console application stopped.");
                Console.WriteLine("Press any key to close this window...");
                Console.ReadKey();
            }
        }
    }
}
