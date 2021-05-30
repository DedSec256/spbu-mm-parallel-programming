using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Threading;
using NUnit.Framework;
using ProducersConsumers.Storage;
using ProducersConsumers.Workers;

namespace ProducersConsumersTest
{
    [TestFixture]
    public class ConsumerTest
    {
        private IProducerConsumerBuffer<string> _buffer;

        [SetUp]
        public void Startup()
        {
            _buffer = new ProducerConsumerList<string>();
        }

        [Test]
        public void WhenConsumerNotStartedThenStoppingItThrowExceptionTest()
        {
            var consumer = new Consumer<string>(_buffer, item => { });
            Assert.Throws<ThreadStateException>(() => consumer.Stop());
        }

        [Test]
        public void WhenMultipleObjectsAddedToBufferThenConsumerTakeAllTest()
        {
            var testStrings = new HashSet<string> { "Test1", "Test2", "Test3", "Test4" };
            foreach (var item in testStrings)
            {
                _buffer.Add(item);
            }

            var consumedStrings = new ConcurrentQueue<string>();
            var consumer = new Consumer<string>(_buffer, item => { consumedStrings.Enqueue(item); });
            consumer.Start();
            Thread.Sleep(500);
            consumer.Stop();

            Assert.AreEqual(testStrings.Count, consumedStrings.Count);
            foreach (var item in consumedStrings)
            {
                Assert.IsTrue(testStrings.Contains(item));
            }
        }

        [Test]
        public void WhenMultipleConsumersRunThenTheyTakeAllObjectsTest()
        {
            var testStrings = new HashSet<string> { "Test1", "Test2", "Test3", "Test4" };
            foreach (var item in testStrings)
            {
                _buffer.Add(item);
            }

            var consumedStrings = new ConcurrentQueue<string>();
            Action<string> consume = item => { consumedStrings.Enqueue(item); Thread.Sleep(100); };
            var consumer1 = new Consumer<string>(_buffer, consume);
            var consumer2 = new Consumer<string>(_buffer, consume);
            consumer1.Start();
            consumer2.Start();
            Thread.Sleep(1000);
            consumer1.Stop();
            consumer2.Stop();

            Assert.AreEqual(testStrings.Count, consumedStrings.Count);
            foreach (var item in consumedStrings)
            {
                Assert.IsTrue(testStrings.Contains(item));
            }
        }
    }
}
