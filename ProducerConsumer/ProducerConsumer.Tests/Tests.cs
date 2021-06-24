using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using NUnit.Framework;

namespace ProducerConsumer.Tests
{
    public class Tests
    {
        [Test]
        public void TestNoConsumers()
        {
            using var cts = new CancellationTokenSource();
            var queue = new QueueWithLock<int>();
            var producer = new Producer<int>(queue, () => 42, cts.Token);

            cts.CancelAfter(3000);
            Thread.Sleep(2000);

            Assert.False(queue.IsEmpty);
        }

        [Test]
        public void TestNoProducers()
        {
            var dataReceived = false;

            using var cts = new CancellationTokenSource();
            var queue = new QueueWithLock<int>();
            var consumers = new Consumer<int>(queue, value => dataReceived = true, cts.Token);

            cts.CancelAfter(3000);

            Assert.True(queue.IsEmpty);
            Assert.False(dataReceived);
        }

        [Test]
        public void TestCancellation()
        {
            using var producerCts = new CancellationTokenSource();
            using var consumerCts = new CancellationTokenSource();
            var queue = new QueueWithLock<int>();
            var producer = new Producer<int>(queue, () => 42, producerCts.Token);
            var consumer = new Consumer<int>(queue, value => Console.Write(""), consumerCts.Token);

            Thread.Sleep(2000);
            producerCts.Cancel();

            WaitWithTimeout(() => queue.IsEmpty, 10000);
        }

        [Test]
        public void TestProducersConsumers()
        {
            var consumer1ReceivedData = false;
            var consumer2ReceivedData = false;

            using var cts = new CancellationTokenSource();
            var ct = cts.Token;

            var queueWithLock = new QueueWithLock<int>();
            var producers = new List<Producer<int>>
            {
                new(queueWithLock, () => 1, ct),
                new(queueWithLock, () => 2, ct)
            };
            var consumers = new List<Consumer<int>>
            {
                new(queueWithLock,
                    value =>
                    {
                        if (value == 1) consumer1ReceivedData = true;
                    }, ct),

                new(queueWithLock, value =>
                {
                    if (value == 2) consumer2ReceivedData = true;
                }, ct)
            };

            WaitWithTimeout(() => consumer1ReceivedData && consumer2ReceivedData, 10000);
        }

        private static void WaitWithTimeout(Func<bool> predicate, int timeout)
        {
            var sw = new Stopwatch();
            sw.Start();

            while (sw.ElapsedMilliseconds < timeout)
                if (predicate())
                    return;

            sw.Stop();
            Assert.False(true);
        }
    }
}
