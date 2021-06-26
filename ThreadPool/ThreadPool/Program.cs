using System;

namespace ThreadPool
{
    internal static class Program
    {
        private static void Main()
        {
            using var tp = new MyThreadPool(4);

            var task = tp.Enqueue(() =>
            {
                Console.WriteLine($"Task 1, threadId {Environment.CurrentManagedThreadId}");
                return 42;
            });

            var taskCont = task.ContinueWith(result =>
            {
                Console.WriteLine($"Task 1 continuation, threadId {Environment.CurrentManagedThreadId}");

                var innerTask = tp.Enqueue(() =>
                {
                    Console.WriteLine($"Task inside another task, threadId {Environment.CurrentManagedThreadId}");
                    return 8;
                });

                return (result + innerTask.Result) * 2;
            });

            tp.Enqueue(taskCont);

            Console.WriteLine(taskCont.Result);
        }
    }
}
