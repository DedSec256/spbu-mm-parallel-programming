package ru.spbu.mm;

import java.util.List;
import java.util.function.Function;

public interface IMyTask<TArg, TReturn> {

    public TReturn getResult() throws InterruptedException;
    public boolean getIsCompleted();
    public <TResult, TNewResult> GenericTask<TResult, TNewResult> continueWith(Function<TResult, TNewResult> func);
    public void run() throws InterruptedException;
}
