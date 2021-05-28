using System;
using System.Collections.Generic;
using NUnit.Framework;
using ThreadPool;

namespace ThreadPoolTest
{
    public class Tests
    {
        [SetUp]
        public void Setup()
        {
        }

        [Test]
        public void SimpleTest()
        {
            using (var tp = new ThreadPool.ThreadPool(8))
            {
                var task = tp.Enqueue(() => 42);

                Assert.AreEqual(42,task.Result);
            }
        }
        
        
        private IMyTask<object> QuickSort(ThreadPool.ThreadPool tp, int[] arr)
        {
            return tp.Enqueue<object>( () =>
            {
                QuickSort(tp,arr, 0, arr.Length - 1);
                return null;
            });
        }
        private void QuickSort(ThreadPool.ThreadPool tp, int[] arr, int left, int right)
        {

            if (right <= left) return;
            
            int lt = left;
            int gt = right;
            var pivot = arr[left];
            int i = left + 1;
            
            while (i <= gt)
            {
                int cmp = arr[i].CompareTo(pivot);
                if (cmp < 0)
                    Swap(arr, lt++, i++);
                else if (cmp > 0)
                    Swap(arr, i, gt--);
                else
                    i++;
            }
		
            var t1 = tp.Enqueue<object>(() =>
            {  
                QuickSort(tp, arr, left, lt - 1);
                return null;
            });

           
            QuickSort(tp, arr, gt + 1, right);

            while (!t1.IsCompleted)
            {
                tp.RunPendingTask();
            }
            
        }
        private void Swap(int[] a, int i, int j)
        {
            var swap = a[i];
            a[i] = a[j];
            a[j] = swap;
        }

        [Test]
        public void QuickSortTest()
        {
            void Shuffler(int[] array)
            {
                var random = new Random();
                int n = array.Length;

                while (n > 1)
                {
                    n--;
                    int k = random.Next(n + 1);
                    int value = array[k];
                    array[k] = array[n];
                    array[n] = value;
                }
            }

            int arraySize = 10000;
            int[] sortedArray = new int[arraySize];
            
            for (int i = 0; i < arraySize; i++)
            {
                sortedArray[i] = i;
            }

            int[] shuffledArray = new int[arraySize];
            Array.Copy(sortedArray,shuffledArray,arraySize);
            
            Shuffler(shuffledArray);

            var tp = new ThreadPool.ThreadPool(8);

            var task = QuickSort(tp, shuffledArray);

            var taskResult = task.Result;

            CollectionAssert.AreEqual(sortedArray,shuffledArray);
            
            tp.Dispose();
            
        }

        [Test]
        public void DisposeWaitAllTest()
        {
            int n = 1000;
            int m = 1000;
            
            //all tasks count to 1000

            var tp = new ThreadPool.ThreadPool(8);

            var root = tp.Enqueue(() => 0);

            var tasks = new List<IMyTask<int>>();
            
            for (int i = 0; i < n; i++)
            {
                var cont = root;
                
                for (int j = 0; j < m; j++)
                {
                    cont = cont.ContinueWith((result) => result + 1);
                    tp.Enqueue(cont);   
                }
                tasks.Add(cont);
                
            }
            
            tp.Dispose();

            foreach (var task in tasks)
            {
                Assert.AreEqual(1000,task.Result);
            }


        }

        [Test]
        public void RunPendingTaskExceptionTest()
        {
            var tp = new ThreadPool.ThreadPool(4);

            Assert.Throws<ApplicationException>(() => tp.RunPendingTask());
            
            tp.Dispose();
        }

        [Test]
        public void NumberOfWorkersTest()
        {
            using var tp = new ThreadPool.ThreadPool(8);
            Assert.AreEqual(8, tp.NumberOfWorkers);
        }

        [Test]
        public void ContinuationExceptionTest()
        {
            using var tp = new ThreadPool.ThreadPool(8);
            var root = tp.Enqueue<object>(() => throw new ArgumentException("Root task exception"));
            var cont = root.ContinueWith<object>((result) => result);
            tp.Enqueue(cont);

            try
            {
                var result = cont.Result;
            }
            catch (AggregateException e)
            {
                
                Assert.AreEqual("Root task exception",e.InnerExceptions[0].Message);
                                
            }
            
        }

        [Test]
        public void EnqueueDisposedExceptionTest()
        {
            var tp = new ThreadPool.ThreadPool(8);
            
            tp.Dispose();

            Assert.Throws<OperationCanceledException>(() =>
            {
                var task = tp.Enqueue(() => 42);

            });
            
        }
        
    }
}