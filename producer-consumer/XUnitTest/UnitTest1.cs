using System;
using Xunit;
using System.Threading;

namespace producer_consumer
{
    public class UnitTest1
    {
        [Fact]
        public void QueueTest()
        {
            using (var tq = new TaskQueue<int>())
            {
                Assert.Equal(0, tq.Size());
            }
        }

        [Fact]
        public void ProducerTest()
        {
            using (var tq = new TaskQueue<int>())
            {
                var pr = new Producer<int>(tq);
                Thread.Sleep(10);
                Assert.Equal(1, tq.Size());
            }
        }

        [Fact]
        public void ConsumerTest()
        {
            using (var tq = new TaskQueue<int>())
            {
                var pr = new Producer<int>(tq);
                var cs = new Consumer<int>(tq);
                Thread.Sleep(10);
                Assert.Equal(0, tq.Size());
            }
        }

        [Fact]
        public void MultipleProducersTest()
        {
            using (var tq = new TaskQueue<int>())
            {
                var pr1 = new Producer<int>(tq);
                var pr2 = new Producer<int>(tq);
                Thread.Sleep(50);
                Assert.Equal(2, tq.Size());
            }
        }

        [Fact]
        public void MultipleConsumersTest()
        {
            using (var tq = new TaskQueue<int>())
            {
                var pr1 = new Producer<int>(tq);
                var pr2 = new Producer<int>(tq);
                var cs1 = new Consumer<int>(tq);
                var cs2 = new Consumer<int>(tq);
                Thread.Sleep(50);
                Assert.Equal(0, tq.Size());
            }
        }

        [Fact]
        public void MoreProducersTest()
        {
            using (var tq = new TaskQueue<int>())
            {
                var pr1 = new Producer<int>(tq);
                var pr2 = new Producer<int>(tq);
                var cs = new Consumer<int>(tq);
                Thread.Sleep(50);
                Assert.Equal(1, tq.Size());
            }
        }

        [Fact]
        public void MoreConsumersTest()
        {
            using (var tq = new TaskQueue<int>())
            {
                var pr = new Producer<int>(tq);
                var cs1 = new Consumer<int>(tq);
                var cs2 = new Consumer<int>(tq);
                Thread.Sleep(50);
                Assert.Equal(0, tq.Size());
            }
        }
    }
}
