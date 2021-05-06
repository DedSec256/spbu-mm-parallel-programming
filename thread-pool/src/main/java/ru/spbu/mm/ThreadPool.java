package ru.spbu.mm;

import java.util.ArrayList;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class ThreadPool {

    private final ArrayList<WorkerThread> workerThreads = new ArrayList<WorkerThread>();
    private BlockingQueue<IMyTask<?, ?>> taskQueue;

    private class WorkerThread extends Thread {
        IMyTask<?, ?> task;
        private boolean busy = false;
        private BlockingQueue<IMyTask<?, ?>> taskQueue;

        public void setTask(IMyTask<?, ?> task) {
            this.task = task;
        }
        public void setTaskQueue(BlockingQueue<IMyTask<?, ?>> taskQueue) {
            this.taskQueue = taskQueue;
        }

        public void run() {
            while (!this.isInterrupted()) {
                try {
                    IMyTask<?, ?> curTask = this.taskQueue.take();
                    curTask.run();
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

    private void findAssigneeAndSubmit(IMyTask<?, ?> task) {
        try {
            this.taskQueue.put(task);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    void releaseThreads() {
        for (Thread workerThread : this.workerThreads) {
            workerThread.interrupt();
        }
    }

    public void submit(IMyTask<?, ?> task) {
        new Thread(() -> {
            this.findAssigneeAndSubmit(task);
        }).start();
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
