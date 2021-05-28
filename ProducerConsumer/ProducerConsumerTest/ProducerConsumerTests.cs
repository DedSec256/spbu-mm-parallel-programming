using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using NUnit.Framework;
using ProducerConsumer;

namespace ProducerConsumerTest
{
    public class Tests
    {
        [SetUp]
        public void Setup()
        {
        }

        [Test]
        public void TestEmptyConsumers()
        {
            using (var cts = new CancellationTokenSource())
            {
                using (var buffer = new Buffer<int>(cts))
                {
                    var producers = new List<Producer<int>>();

                    for (int i = 0; i < 4; i++)
                    {
                        producers.Add(new Producer<int>(buffer,()=>42,cts.Token));
                    }
                    
                    Thread.Sleep(5000);
                    
                    buffer.Cancel();
                    
                    foreach (var producer in producers)
                    {
                        producer.Wait();
                    }

                    Assert.False(buffer.isEmpty());
                }
            }
            
        }

        [Test]
        public void TestEmptyProducers()
        {
            int[] consumerDestinationArray = new int [4] {0, 0, 0, 0};
            
            using (var cts = new CancellationTokenSource())
            {
                using (var buffer = new Buffer<int>(cts))
                {
                    var consumers = new List<Consumer<int>>();

                    for (int i = 0; i < 4; i++)
                    {
                        int j = i;
                        consumers.Add(new Consumer<int>(buffer, (result) =>
                        {
                            consumerDestinationArray[i] = 444;
                            return null;
                        },cts.Token));
                    }
                    
                    Thread.Sleep(5000);
                    
                    buffer.Cancel();
                    
                    foreach (var consumer in consumers)
                    {
                        consumer.Wait();
                    }

                    Assert.True(buffer.isEmpty());
                }
            }

            foreach (var elem in consumerDestinationArray)
            {
                Assert.True(elem == 0);
            }
        }

        private IEnumerable<int> TestSeveralConsumersProducersGenerator(int producerNumber)
        {
            for (int i = 5 * producerNumber; i < 5 * producerNumber + 5; i++)
            {
                yield return i;
            }
        }

        [Test]
        public void TestSeveralConsumersProducers()
        {
            int numberOfProducers = 3;
            int[] consumerFinished = new int[3];
            int[] consumerDestinationArray = new int [numberOfProducers * 5];
            int[] expectedDestinationArray = new int [numberOfProducers * 5];

            for (int i = 0; i < numberOfProducers * 5; i++)
            {
                expectedDestinationArray[i] = i;
            }
            
            using (var cts = new CancellationTokenSource())
            {
                using (var buffer = new Buffer<Tuple<int,int>>(cts))
                {

                    var producers = new List<Producer<Tuple<int,int>>>();

                    for (int i = 0; i < numberOfProducers; i++)
                    {
                        int local_i = i;
                        var enumerator = TestSeveralConsumersProducersGenerator(local_i).GetEnumerator();
                        
                        int j = local_i * 5;
                        
                        producers.Add(new Producer<Tuple<int,int>>(buffer, () =>
                        {
                            if (enumerator.MoveNext())
                            {
                                j += 1;
                                return new Tuple<int,int>(j - 1,enumerator.Current);
                            }

                            enumerator.Dispose();
                            return new Tuple<int, int>(-1,42);
                            
                        },cts.Token));
                    }
                    var consumers = new List<Consumer<Tuple<int,int>>>();

                    for (int i = 0; i < 3; i++)
                    {
                        int consumerId = i;
                        consumers.Add(new Consumer<Tuple<int,int>>(buffer, (result) =>
                        {
                            var (item1, item2) = result;
                            if (item1 == -1)
                            {
                                consumerFinished[consumerId] = 1;
                                return null;
                            }
                            
                            consumerDestinationArray[item1] = item2;
                            return null;
                            
                        },cts.Token));
                    }

                    while (consumerFinished.Any(elem => elem == 0))
                    {
                    }

                    buffer.Cancel();
                    
                    foreach (var consumer in consumers)
                    {
                        consumer.Wait();
                    }

                    foreach (var producer in producers)
                    {
                        producer.Wait();
                    }

                    CollectionAssert.AreEqual(expectedDestinationArray,consumerDestinationArray);
                }
            }
        }
    }
}