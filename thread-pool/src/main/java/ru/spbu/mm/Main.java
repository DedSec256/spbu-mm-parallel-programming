package ru.spbu.mm;

import java.util.ArrayList;
import java.util.function.Function;

public class Main {
    public static void main(String[] args) throws InterruptedException {
        ThreadPool threadPool = new ThreadPool(3);

        Function<Integer, Integer> funcMul2 = new Functions.Mul2();
        Function<Integer, String> funcInt2Str = new Functions.Integer2String();
        Function<String, String> funcStrWrap = new Functions.StringWrap();
        Function<Integer, Integer> funcIdInteger = new Functions.IdFunc<Integer>();

        ArrayList<GenericTask<Integer, Integer>> tasks = new ArrayList<>();
        for (int i = 0; i < 250; i++) {
            tasks.add(new GenericTask<>(funcIdInteger, i));
        }
        for (GenericTask<Integer, Integer> task : tasks) {
            threadPool.submit(task);
        }
        for (GenericTask<Integer, Integer> task : tasks) {
            System.out.println(task.getResult());
        }
        threadPool.releaseThreads();
    }
}