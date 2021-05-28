package ru.spbu.mm;

import java.util.ArrayList;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.locks.ReentrantLock;


public class ThreadPool {

    private final ArrayList<WorkerThread> workerThreads = new ArrayList<WorkerThread>();
    private BlockingQueue<IMyTask<?, ?>> taskQueue;

    private class WorkerThread extends Thread {
        IMyTask<?, ?> task;
        private ReentrantLock safeToInterrupt;
        private BlockingQueue<IMyTask<?, ?>> taskQueue;
        private volatile boolean workerInterrupt = false;

        public WorkerThread() {
            this.safeToInterrupt = new ReentrantLock();
        }

        public void setTask(IMyTask<?, ?> task) {
            this.task = task;
        }
        public void setTaskQueue(BlockingQueue<IMyTask<?, ?>> taskQueue) {
            this.taskQueue = taskQueue;
        }

        public void interruptWorker() {
            this.workerInterrupt = true;
            this.safeToInterrupt.lock();
            try {
                this.interrupt();
            } finally {
                this.safeToInterrupt.unlock();
            }
        }

        public void run() {
            while (!this.workerInterrupt) {
                try {
                    IMyTask<?, ?> curTask = this.taskQueue.take();
                    this.safeToInterrupt.lock();
                    try {
                        curTask.run();
                    } finally {
                        this.safeToInterrupt.unlock();
                    }

                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            }
        }
    }

    public ThreadPool(int numberOfThreads) {
        this.taskQueue = new LinkedBlockingQueue<>();
        WorkerThread tempWorker;
        for (int i = 0; i < numberOfThreads; i++) {
            tempWorker = new WorkerThread();
            this.workerThreads.add(tempWorker);
            tempWorker.setTaskQueue(this.taskQueue);
            tempWorker.start();
        }
    }

    private void putTask(IMyTask<?, ?> task) {
        try {
            this.taskQueue.put(task);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    public void releaseThreads() {
        for (WorkerThread workerThread : this.workerThreads) {
            workerThread.interruptWorker();
        }
    }

    public void submit(IMyTask<?, ?> task) {
        this.putTask(task);
    }

    public Integer getNumberOfWorkingThreads() {
        int cnt = 0;
        for (Thread workerThread : this.workerThreads) {
            cnt += (workerThread.getState() == Thread.State.RUNNABLE | workerThread.getState() == Thread.State.WAITING)
                    ? 1 : 0;
        }
        return cnt;
    }
}
