using System.Collections.Generic;
using NUnit.Framework;
using ProducersConsumers.Storage;

namespace ProducersConsumersTest
{
    [TestFixture]
    public class StorageTest
    {
        private IProducerConsumerBuffer<string> _buffer;

        [SetUp]
        public void Startup()
        {
            _buffer = new ProducerConsumerList<string>();
        }

        [Test]
        public void WhenBufferIsEmptyThenCannotTakeItemTest()
        {
            Assert.IsFalse(_buffer.TryTake(out var item));
        }

        [Test]
        public void WhenAddedOneObjectThenItCanBeTakenTest()
        {
            _buffer.Add("Test");
            Assert.IsTrue(_buffer.TryTake(out var item) && item.Equals("Test"));
        }

        [Test]
        public void WhenAddedOneObjectThenOnlyOneObjectCanBeTakenTest()
        {
            _buffer.Add("Test");
            Assert.IsTrue(_buffer.TryTake(out var item));
            Assert.AreEqual("Test", item);
            Assert.IsFalse(_buffer.TryTake(out item));
        }

        [Test]
        public void WhenAddedMultipleObjectsThenAllCanBeTakenTest()
        {
            var testStrings = new HashSet<string> { "Test1", "Test2", "Test3", "Test4" };
            foreach (var item in testStrings)
            {
                _buffer.Add(item);
            }

            while (_buffer.TryTake(out var item))
            {
                Assert.IsTrue(testStrings.Contains(item));
            }
        }
    }
}
