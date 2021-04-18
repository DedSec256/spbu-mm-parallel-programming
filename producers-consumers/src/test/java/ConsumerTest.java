
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

import ru.spbu.mm.Constants;
import ru.spbu.mm.Consumer;
import ru.spbu.mm.SimpleTask;

import java.util.ArrayList;

public class ConsumerTest {
    private static Consumer consumer;
    private final static ArrayList<SimpleTask> tasks = new ArrayList<SimpleTask>();;

    @BeforeAll
    @DisplayName("Create the consumer object")
    static public void createConsumer() {
        SimpleTask task = new SimpleTask(Constants.minTaskPeriod);
        tasks.add(task);
        consumer = new Consumer(tasks, "CnsmrTest", Constants.sleepPeriod);
        assertNotEquals(consumer, null);
    }

    @Test
    @DisplayName("Feed a task to a consumer")
    public void feedTask () {
        consumer.start();
        try {
            while (!tasks.isEmpty()) {
                Thread.sleep(Constants.minTaskPeriod);
            }
        }
        catch (InterruptedException e) {
            System.out.printf("The testing thread was interrupted%n");
        }
    }
}
