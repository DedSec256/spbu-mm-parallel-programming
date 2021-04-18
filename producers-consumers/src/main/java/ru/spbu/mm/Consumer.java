package ru.spbu.mm;

import java.util.ArrayList;

public class Consumer extends Thread {
    final ArrayList<SimpleTask> tasksList;
    String name;
    int period;

    public Consumer(ArrayList<SimpleTask> tasksList, String name, int period) {
        this.tasksList = tasksList;
        this.name = name;
        this.period = period;
    }

    public void run() {
        try {
            while (!this.isInterrupted()) {
                SimpleTask task = null;
                synchronized (this.tasksList) {
                    if (this.tasksList.size() > 0) {
                        task = this.tasksList.get(0);
                        this.tasksList.remove(0);
                        System.out.printf("%s has taken a task, tasks amount: %d%n", this.name, this.tasksList.size());
                    }
                }
                if (task != null) {
                    task.performTask();
                }
            }
        } catch (InterruptedException e) {
            System.out.printf("Consumer %s thread was terminated%n", this.name);
        }
    }
}

