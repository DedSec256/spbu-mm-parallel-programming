import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.assertEquals;
import org.junit.jupiter.api.Disabled;

import ru.spbu.mm.Functions;
import ru.spbu.mm.GenericTask;
import ru.spbu.mm.ThreadPool;
import ru.spbu.mm.Constants;

import java.util.ArrayList;
import java.util.function.Function;


public class ThreadPoolTest {
    private static ThreadPool threadPool;
    private static Function<Integer, Integer> funcMul2 = new Functions.Mul2();
    private static Function<Integer, String> funcInt2Str = new Functions.Integer2String();
    private static Function<String, String> funcStrWrap = new Functions.StringWrap();
    private static Function<Integer, Integer> funcIdInteger = new Functions.IdFunc<Integer>();
    private static final Integer threadsNum = 5;
    private static final Integer tasksNum = 25;
    private static final Integer randomNumber = 10;
    private static final String randomNumberString = "10";
    private static final String randomNumberStringDecorated = "|10|";

    @BeforeEach
    @DisplayName("Create the threadPool object and check the amount of threads")
    public void createThreadPool() {
        threadPool = new ThreadPool(threadsNum);
        assertEquals(threadsNum, threadPool.getNumberOfWorkingThreads());
    }

    private void releaseThreads() {
        threadPool.releaseThreads();
        try {
            Thread.sleep(Constants.periodForThreadsRelease);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Test
    @DisplayName("Perform single task test")
    public void addOneTask() {
        GenericTask<Integer, Integer> task = new GenericTask<>(funcIdInteger, randomNumber);
        threadPool.submit(task);
        assertEquals(task.getResult(), randomNumber);
    }

    @Test
    @DisplayName("Perform single task and release thread from the threadPool")
    public void addOneTaskAndInterrupt() {
        GenericTask<Integer, Integer> task = new GenericTask<>(funcIdInteger, randomNumber);
        threadPool.submit(task);
        try {
            Thread.sleep(Constants.sleepPeriod / 2);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        this.releaseThreads();
        assertEquals(task.getResult(), randomNumber);
        assertEquals(threadPool.getNumberOfWorkingThreads(), 0);
    }

    @Test
    @DisplayName("Perform multiple tasks test")
    public void addMoreTasks() {
        ArrayList<GenericTask<Integer, Integer>> tasks = new ArrayList<>();
        ArrayList<Integer> results = new ArrayList<>();
        ArrayList<Integer> canonicalResults = new ArrayList<>();
        for (int i = 0; i < tasksNum; i++) {
            tasks.add(new GenericTask<>(funcIdInteger, i));
            canonicalResults.add(i);
        }
        for (GenericTask<Integer, Integer> task : tasks) {
            threadPool.submit(task);
        }
        for (int i = 0; i < tasksNum; i++) {
            results.add(tasks.get(i).getResult());
        }
        assertEquals(results, canonicalResults);
    }

    @Test
    @DisplayName("Single continueWith test")
    public void singleContinueWith() {
        GenericTask<Integer, Integer> task = new GenericTask<>(funcIdInteger, randomNumber);
        GenericTask<Integer, String> continueTask = task.continueWith(funcInt2Str);
        threadPool.submit(task);
        threadPool.submit(continueTask);
        assertEquals(continueTask.getResult(), randomNumberString);
    }

    @Test
    @DisplayName("Multiple continueWith test")
    public void multipleContinueWith() {
        GenericTask<Integer, Integer> task = new GenericTask<>(funcIdInteger, randomNumber);
        GenericTask<Integer, String> continueTask = task.continueWith(funcInt2Str);
        GenericTask<String, String> continueContinueTask = task.continueWith(funcStrWrap);
        threadPool.submit(task);
        threadPool.submit(continueTask);
        threadPool.submit(continueContinueTask);
        assertEquals(continueTask.getResult(), randomNumberString);
        assertEquals(continueContinueTask.getResult(), randomNumberStringDecorated);
        threadPool.releaseThreads();
    }
}
