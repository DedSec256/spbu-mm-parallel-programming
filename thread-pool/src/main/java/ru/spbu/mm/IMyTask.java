package ru.spbu.mm;

import java.util.List;
import java.util.function.Function;

public interface IMyTask<TArg, TReturn> {

    public TReturn getResult() throws InterruptedException;
    public boolean getIsCompleted();
    public <TNewResult> Object continueWith(Function<TReturn, TNewResult> func) throws InterruptedException;
    public void run() throws InterruptedException;
}
