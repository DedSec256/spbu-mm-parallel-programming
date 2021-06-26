using System;
using System.Linq;
using System.Threading;
using NUnit.Framework;

namespace ThreadPool.Tests
{
    public class Tests
    {
        [Test]
        public void SimpleTest()
        {
            using var tp = new MyThreadPool(8);
            var task = tp.Enqueue(() => 42);

            Assert.AreEqual(42, task.Result);
        }

        [Test]
        public void DisposeWaitAllTest()
        {
            using var tp = new MyThreadPool(8);

            var root = tp.Enqueue(() => 0);
            const int n = 10;

            var lastTask = Enumerable.Range(0, n)
                .Aggregate(root, (t, _) => t.ContinueWith(result =>
                {
                    Thread.Sleep(1000);
                    return result + 1;
                }));

            tp.Dispose();

            Assert.AreEqual(n, lastTask.Result);
        }

        [Test]
        public void IsCompletedTest()
        {
            using var tp = new MyThreadPool(8);
            var task = tp.Enqueue(() =>
            {
                Thread.Sleep(5000);
                return 42;
            });

            Assert.False(task.IsCompleted);

            Assert.AreEqual(42, task.Result);
            Assert.True(task.IsCompleted);
        }

        [Test]
        public void ContinuationExceptionTest()
        {
            using var tp = new MyThreadPool(8);
            var root = tp.Enqueue<object>(() => throw new ArgumentException("Root task exception"));
            var cont = root.ContinueWith(result => result);
            tp.Enqueue(cont);

            try
            {
                var _ = cont.Result;
            }
            catch (AggregateException e)
            {
                Assert.AreEqual("Root task exception", e.InnerExceptions[0].Message);
            }
        }

        [Test]
        public void EnqueueDisposedExceptionTest()
        {
            using var tp = new MyThreadPool(8);
            tp.Dispose();

            Assert.Throws<OperationCanceledException>(() => tp.Enqueue(() => 42));
        }

        [Test]
        public void NumberOfWorkersTest()
        {
            using var tp = new MyThreadPool(8);
            Assert.AreEqual(8, tp.NumberOfWorkers);

            tp.Dispose();
            Assert.Zero(tp.NumberOfWorkers);
        }

        [Test]
        public void TaskRunOnceTest()
        {
            var received = 0;

            using var tp = new MyThreadPool(8);
            var task = tp.Enqueue(() => ++received);

            var _ = task.Result;
            var __ = task.Result;

            Assert.AreEqual(1, received);
            Assert.AreEqual(1, task.Result);
        }

        [Test]
        public void InnerTaskTest()
        {
            using var tp = new MyThreadPool(8);
            var task = tp.Enqueue(() =>
            {
                var innerTask = tp.Enqueue(() => 8);
                return 42 + innerTask.Result;
            });

            Assert.AreEqual(50, task.Result);
        }
    }
}
