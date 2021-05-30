using System.Collections.Generic;
using System.Threading;
using NUnit.Framework;
using ProducersConsumers.Storage;
using ProducersConsumers.Workers;

namespace ProducersConsumersTest
{
    [TestFixture]
    public class ProducerTest
    {
        private IProducerConsumerBuffer<string> _buffer;

        [SetUp]
        public void Startup()
        {
            _buffer = new ProducerConsumerList<string>();
        }

        [Test]
        public void WhenProducerNotStartedThenStoppingItThrowExceptionTest()
        {
            var producer = new Producer<string>(_buffer, () => "Test");
            Assert.Throws<ThreadStateException>(() => producer.Stop());
        }

        [Test]
        public void WhenProducerAddObjectsToBufferThenTheyCanBeTaken()
        {
            var producer = new Producer<string>(_buffer, () => "Test");
            producer.Start();
            Thread.Sleep(100);
            producer.Stop();
            Assert.IsTrue(_buffer.TryTake(out var item));
            Assert.AreEqual("Test", item);
        }

        [Test]
        public void WhenMultipleProducerAddObjectsToBufferThenTheyCanBeTaken()
        {
            var testStrings = new[] { "Test1", "Test2" };
            var producer1 = new Producer<string>(_buffer, () => testStrings[0]);
            var producer2 = new Producer<string>(_buffer, () => testStrings[1]);
            producer1.Start();
            producer2.Start();
            Thread.Sleep(100);
            producer1.Stop();
            producer2.Stop();

            var producedStrings = new HashSet<string>();
            while (producedStrings.Count < 2 && _buffer.TryTake(out var item))
            {
                producedStrings.Add(item);
            }

            foreach (var item in testStrings)
            {
                Assert.IsTrue(producedStrings.Contains(item));
            }
        }
    }
}
