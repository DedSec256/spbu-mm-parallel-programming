package ru.spbu.mm;

import java.util.function.Function;

public class Functions {

    public static class IdFunc<T> implements Function<T, T> {
        public T apply(T stuff) {
            try {
                Thread.sleep(Constants.sleepPeriod);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            return stuff;
        }
    }

    public static class Mul2 implements Function<Integer, Integer> {
        public Integer apply(Integer num) {
            try {
                Thread.sleep(Constants.sleepPeriod);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            return num * 2;
        }
    }

    public static class Integer2String implements Function<Integer, String> {
        public String apply(Integer num) {
            try {
                Thread.sleep(Constants.sleepPeriod);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            return Integer.toString(num);
        }
    }

    public static class StringWrap implements Function<String, String> {
        public String apply(String str) {
            try {
                Thread.sleep(Constants.sleepPeriod);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            return "|" + str + "|";
        }
    }
}
