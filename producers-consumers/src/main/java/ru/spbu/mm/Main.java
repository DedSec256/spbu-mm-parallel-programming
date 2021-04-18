package ru.spbu.mm;

import java.util.ArrayList;

public class Main {
    public static void main(String[] args) {
        int producersAmt = Integer.parseInt(args[0]);
        int consumersAmt = Integer.parseInt(args[1]);

        ArrayList<Producer> producers = new ArrayList<Producer>();
        ArrayList<Consumer> consumers = new ArrayList<Consumer>();
        ArrayList<SimpleTask> tasks = new ArrayList<SimpleTask>();
        Producer tempProducer;
        Consumer tempConsumer;

        // Define and run producers
        for (int i = 0; i < producersAmt; i++) {
            tempProducer = new Producer(tasks, "Producer#" + Integer.toString(i), Constants.sleepPeriod,
                    Constants.minTaskPeriod, Constants.maxTaskPeriod);
            producers.add(tempProducer);
            tempProducer.start();
        }

        // Define and run consumers
        for (int i = 0; i < consumersAmt; i++) {
            tempConsumer = new Consumer(tasks, "Consumer#" + Integer.toString(i), Constants.sleepPeriod);
            consumers.add(tempConsumer);
            tempConsumer.start();
        }

        System.out.println("Press Enter key to continue...");

        // Wait for the Enter key and send interrupt signal to the threads
        try
        {
            System.in.read();
            System.out.println("Sending interrupt signals to producers and consumers");
            for (Producer producer : producers) {
                producer.interrupt();
            }
            for (Consumer consumer : consumers) {
                consumer.interrupt();
            }
        }
        catch(Exception e)
        {
            System.out.println(e.toString());
        }
    }
}