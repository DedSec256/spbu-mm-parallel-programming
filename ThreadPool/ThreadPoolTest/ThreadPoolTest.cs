using System;
using System.Collections.Generic;
using System.Threading;
using NUnit.Framework;
using ThreadPool;
using ThreadPool.MyTask;

namespace ThreadPoolTest
{
    [TestFixture]
    public class ThreadPoolTest
    {
        private const int ThreadPoolSize = 4;
        private MyThreadPool _tp;

        [SetUp]
        public void Startup()
        {
            _tp = new MyThreadPool(ThreadPoolSize);
        }

        [TearDown]
        public void Cleanup()
        {
            try
            {
                _tp.Dispose();
            }
            catch (ObjectDisposedException)
            {
                // Ignored for tests with disposing ThreadPool
            }
        }

        [Test]
        public void WhenTaskNotEnqueuedThenItNotCompletedTest()
        {
            using var task = new MyTask<int>(() => 2 + 2);
            Assert.IsFalse(task.IsCompleted);
        }

        [Test]
        public void WhenTaskResultGotThenTaskCompletedTest()
        {
            using var task = new MyTask<int>(() => 2 + 2);
            _tp.Enqueue(task);
            var taskResult = task.Result;
            Assert.IsTrue(task.IsCompleted);
        }

        [Test]
        public void CheckTaskResultCorrectnessTest()
        {
            using var task = new MyTask<int>(() => 2 + 2);
            _tp.Enqueue(task);
            Assert.AreEqual(4, task.Result);
        }

        [Test]
        public void WhenDisposeThreadPoolTwiceThenObjectDisposedExceptionThrownTest()
        {
            _tp.Dispose();
            Assert.Throws<ObjectDisposedException>(() => _tp.Dispose());
        }

        [Test]
        public void WhenAddNullTaskThenArgumentNullExceptionThrownTest()
        {
            Assert.Throws<ArgumentNullException>(() => _tp.Enqueue((IMyTask<int>) null));
        }

        [Test]
        public void WhenEnqueuedTaskFailThenAggregateExceptionThrownTest()
        {
            using var task = new MyTask<int>(() => ((IMyTask<int>) null).Result);
            _tp.Enqueue(task);
            var exception = Assert.Throws<AggregateException>(() => { var taskResult = task.Result; });
            Assert.IsInstanceOf<NullReferenceException>(exception.InnerException);
        }
        
        [Test]
        public void WhenAggregateExceptionThrownInTaskThenTaskResultThrowSameAggregateExceptionTest()
        {
            var exceptions = new HashSet<Exception>
            {
                new NullReferenceException(),
                new ArithmeticException(),
            };
            using var task = new MyTask<int>(() => throw new AggregateException(exceptions));
            _tp.Enqueue(task);
            var aggregateException = Assert.Throws<AggregateException>(() => { var taskResult = task.Result; });
            foreach (var innerException in aggregateException.Flatten().InnerExceptions)
            {
                Assert.IsTrue(exceptions.Contains(innerException));
            }
        }

        [Test]
        public void WhenEnqueuedMoreTasksThanThreadPoolSizeThenTasksFinishWithCorrectResultTest()
        {
            const int tasksCount = 2 * ThreadPoolSize;
            
            var tasks = new MyTask<int>[tasksCount];
            for (int i = 0; i < tasksCount; i++)
            {
                tasks[i] = new MyTask<int>(() => { Thread.Sleep(500); return 2 + 2; });
            }
            
            foreach (var task in tasks)
            {
                _tp.Enqueue(task);
            }
            Assert.AreEqual(ThreadPoolSize, _tp.Size);
            _tp.Dispose();
            foreach (var task in tasks)
            {
                Assert.AreEqual(4, task.Result);
                task.Dispose();
            }
        }

        [Test]
        public void AddTasksInParallelTest()
        {
            const int parallelThreadsCount = 40;
            var threads = new List<Thread>();
            for (var i = 0; i < parallelThreadsCount; i++)
            {
                var j = i;
                var thread = new Thread(() =>
                {
                    using var task = new MyTask<int>(() => j);
                    _tp.Enqueue(task);
                    Assert.AreEqual(j, task.Result);
                });
                threads.Add(thread);
                thread.Start();
            }

            threads.ForEach(thread => thread.Join());
        }

        [Test]
        public void CheckThreadsCountInEmptyThreadPoolTest()
        {
            Assert.AreEqual(ThreadPoolSize, _tp.Size);
        }
        
        [Test]
        public void CheckThreadsCountWhenEnqueuedLessThanThreadPoolSizeTasksTest()
        {
            const int tasksCount = ThreadPoolSize - 1;
            var tasks = new MyTask<int>[tasksCount];
            for (int i = 0; i < tasksCount; i++)
            {
                tasks[i] = new MyTask<int>(() => { Thread.Sleep(500); return 2 + 2; });
            }
            
            foreach (var task in tasks)
            {
                _tp.Enqueue(task);
            }
            Assert.AreEqual(ThreadPoolSize, _tp.Size);
            _tp.Dispose();
            foreach (var task in tasks)
            {
                task.Dispose();
            }
        }

        [Test]
        public void CheckThreadsCountWhenEnqueuedMoreThanThreadPoolSizeTasksTest()
        {
            const int tasksCount = ThreadPoolSize + 1;
            var tasks = new MyTask<int>[tasksCount];
            for (int i = 0; i < tasksCount; i++)
            {
                tasks[i] = new MyTask<int>(() => { Thread.Sleep(500); return 2 + 2; });
            }
            
            foreach (var task in tasks)
            {
                _tp.Enqueue(task);
            }
            Assert.AreEqual(ThreadPoolSize, _tp.Size);
            _tp.Dispose();
            foreach (var task in tasks)
            {
                task.Dispose();
            }
        }
        
        [Test]
        public void WhenThreadPoolDisposedAfterEnqueueingTaskThenTaskCorrectlyFinishTest()
        {
            var task = new MyTask<int>(() => { Thread.Sleep(1000); return 2 + 2; });
            _tp.Enqueue(task);
            _tp.Dispose();
            Assert.AreEqual(4, task.Result);
            task.Dispose();
        }

        [Test]
        public void WhenEnqueueTaskIntoDisposedThreadPoolThenObjectDisposedExceptionThrownTest()
        {
            _tp.Dispose();
            using var task = new MyTask<int>(() => 2 + 2);
            Assert.Throws<ObjectDisposedException>(() => _tp.Enqueue(task));
        }

        [Test]
        public void CheckContinuationTaskUseResultOfBaseTaskTest()
        {
            using var taskA = new MyTask<string>(() => "A");
            using var taskB = taskA.ContinueWith(a => $"{a}B");
            _tp.Enqueue(taskA);
            _tp.Enqueue(taskB);
            Assert.AreEqual("A", taskA.Result);
            Assert.AreEqual("AB", taskB.Result);
        }

        [Test]
        public void WhenContinueWithCalledMultipleTimesThenAllContinuationsUseResultOfBaseTaskTest()
        {
            using var taskA = new MyTask<string>(() => "A");
            using var taskB = taskA.ContinueWith(a => $"{a}B");
            using var taskC = taskA.ContinueWith(a => $"{a}C");
            _tp.Enqueue(taskA);
            _tp.Enqueue(taskB);
            _tp.Enqueue(taskC);
            Assert.AreEqual("AB", taskB.Result);
            Assert.AreEqual("AC", taskC.Result);
        }

        [Test]
        public void WhenContinueContinuationEnqueuedBeforeBaseTaskThenInWillWaitBaseTaskTest()
        {
            using var taskA = new MyTask<string>(() => "A");
            using var taskB = taskA.ContinueWith(a => $"{a}B");
            _tp.Enqueue(taskB);
            Thread.Sleep(500);
            Assert.IsFalse(taskB.IsCompleted);
            _tp.Enqueue(taskA);
            _tp.Dispose();
        }
    }

    [TestFixture]
    public class TestHugeThreadPool
    {
        [Test]
        public void RunLotsOfThreadsTest()
        {
            const int tpSize = 42;
            const int tasksCount = 10;
            using var tp = new MyThreadPool(tpSize);
            var tasks = new List<IMyTask<int>>();
            for (var i = 0; i < tasksCount; i++)
            {
                var task = new MyTask<int>(() =>
                {
                    Thread.Sleep(100);
                    return 2 + 2;
                });
                tp.Enqueue(task);
                tasks.Add(task);
            }

            foreach (var task in tasks)
            {
                Assert.AreEqual(4, task.Result);
                task.Dispose();
            }
        }
    }
}
