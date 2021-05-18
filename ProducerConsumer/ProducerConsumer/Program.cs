using System;
using System.Collections.Generic;
using System.Net.Http;
using System.Text.Json;
using System.Threading;

namespace ProducerConsumer
{
    class Program
    {
        static void RandomJoke()
        {
            using (var cts = new CancellationTokenSource())
            {
                using (var buffer = new Buffer<int>(cts))
                {
                    
                    var producers = new List<Producer<int>>();

                    for (int i = 0; i < 3; i++)
                    {
                        producers.Add(new Producer<int>(buffer, () => 42, cts.Token));
                    }

                    var consumers = new List<Consumer<int>>();

                    for (int i = 0; i < 2; i++)
                    {
                        var client = new HttpClient();

                        int j = i;
                        var token = cts.Token;
                        
                        consumers.Add(new Consumer<int>(buffer, (result) =>
                        {
                            //two co-prime numbers
                            if(j == 0) Thread.Sleep(11*503);
                            else Thread.Sleep(13*501);
                            
                            var request = new HttpRequestMessage
                            {
                                Method = HttpMethod.Get,
                                RequestUri = new Uri("https://official-joke-api.appspot.com/jokes/random"),
                            };

                            
    
                            using (var response = client.Send(request))
                            {
                                try
                                {
                                    response.EnsureSuccessStatusCode();
                                    
                                    var body = response.Content.ReadAsStringAsync(token).Result;
                                    var jsonDocument = JsonDocument.Parse(body);
                                    
                                    var setup = jsonDocument.RootElement.GetProperty("setup");
                                    var punchline = jsonDocument.RootElement.GetProperty("punchline");
                                    
                                    Console.WriteLine("-------------");
                                    Console.WriteLine(setup);
                                    Console.WriteLine(punchline);
                                }
                                catch (HttpRequestException ex)
                                {
                    
                                }
                            }

                            return null;
                        }, cts.Token));
                    }

                    Console.WriteLine("Press any key to stop the world.");
                    Console.ReadKey();
                    Console.WriteLine("\nLet me finish some jokes");

                    buffer.Cancel();

                    foreach (var producer in producers)
                    {
                        producer.Wait();
                    }

                    foreach (var consumer in consumers)
                    {
                        consumer.Wait();
                    }
                    
                }
            }
        }
        // main method just tells a random joke from the corresponding API
        static void Main(string[] args)
        {
            RandomJoke();
        }
    }
}