using System;
using System.Threading;

namespace ThreadPool
{
    class Program
    {
        static void Main(string[] args)
        {

            var tp = new ThreadPool(4);

            var task = tp.Enqueue(() =>
            {
                Console.WriteLine($"Hey, I am a task and I am inside a " +
                                  $"threadpool run by {Thread.CurrentThread.ManagedThreadId}!");
                return 42;
            });

            var taskCont = task.ContinueWith((result) =>
            {
                Console.WriteLine($"Hey, I am a task continuation and I am inside a " +
                                  $"threadpool run by {Thread.CurrentThread.ManagedThreadId}! I create a continuation!");

                var innerTask = tp.Enqueue(() =>
                {
                    Console.WriteLine("I am a task inside another task inside a threadpool! " +
                                      $"I am run by {Thread.CurrentThread.ManagedThreadId}");
                    return 8;
                });
                
                return (result + innerTask.Result) * 2;
            });

            tp.Enqueue(taskCont);
            
            Console.WriteLine(taskCont.Result);
            
            tp.Dispose();
            
        }
    }
}