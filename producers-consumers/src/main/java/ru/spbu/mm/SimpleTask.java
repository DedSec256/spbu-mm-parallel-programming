package ru.spbu.mm;

public class SimpleTask {
    int timeOutMs;

    public SimpleTask(int timeOutMs) {
        this.timeOutMs = timeOutMs;
    }

    public void performTask() throws InterruptedException {
        Thread.sleep(this.timeOutMs);
    }
}
