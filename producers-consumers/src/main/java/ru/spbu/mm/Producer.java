package ru.spbu.mm;

import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;

public class Producer extends Thread {
    final ArrayList<SimpleTask> tasksList;
    String name;
    int sleepPeriod;
    int minTaskPeriod;
    int maxTaskPeriod;

    public Producer(ArrayList<SimpleTask> tasksList, String name, int sleepPeriod, int minTaskPeriod, int maxTaskPeriod) {
        this.tasksList = tasksList;
        this.name = name;
        this.sleepPeriod = sleepPeriod;
        this.minTaskPeriod = minTaskPeriod;
        this.maxTaskPeriod = maxTaskPeriod;
    }

    public void run() {
        try {
            while (!this.isInterrupted()) {
                int timeout = ThreadLocalRandom.current().nextInt(this.minTaskPeriod, this.maxTaskPeriod);
                SimpleTask task = new SimpleTask(timeout);

                synchronized (this.tasksList) {
                    this.tasksList.add(task);
                    System.out.printf("%s has put a task, tasks amount: %d%n", this.name, this.tasksList.size());
                }
                Thread.sleep(this.sleepPeriod);
            }
        } catch (InterruptedException e) {
            System.out.printf("Producer %s thread was terminated%n", this.name);
        }
    }
}
