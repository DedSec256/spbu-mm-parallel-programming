package ru.spbu.mm;

import java.util.concurrent.CountDownLatch;
import java.util.function.Function;

public class GenericTask<TArg, TReturn> implements IMyTask<TArg, TReturn> {
    private TReturn result;
    private TArg argument;
    private Function<TArg, TReturn> function;
    private volatile boolean isCompleted;
    private final CountDownLatch countDown = new CountDownLatch(1);
    private ArgumentType argumentType;
    private GenericTask<?, TArg> precursor;

    public void run() {
        if (this.argumentType == ArgumentType.DYNAMIC) {
            this.argument = this.precursor.getResult();
        }
        this.result = this.function.apply(this.argument);
        this.countDown.countDown();
        this.isCompleted = true;
    }

    public GenericTask(Function<TArg, TReturn> function, TArg argument) {
        this.function = function;
        this.argument = argument;
        this.argumentType = ArgumentType.PREDEFINED;
    }

    public GenericTask(Function<TArg, TReturn> function, GenericTask<?, TArg> task) {
        this.function = function;
        this.precursor = task;
        this.argumentType = ArgumentType.DYNAMIC;
    }

    public boolean getIsCompleted() {
        return this.isCompleted;
    }

    public TReturn getResult() {
        try {
            this.countDown.await();
        } catch (InterruptedException e) {
            System.out.println("Interrupt during getting result");
            Thread.currentThread().interrupt();
        }
        return this.result;
    }

    public <TNewResult> GenericTask<TReturn, TNewResult> continueWith(Function<TReturn, TNewResult> func) {
        GenericTask<TReturn, TNewResult> newTask = new GenericTask<>(func, this);
        return newTask;
    }
}
