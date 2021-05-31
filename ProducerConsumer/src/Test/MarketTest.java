package Test;
import Solution.Market;
import junit.framework.TestCase;

import java.awt.*;
import java.awt.event.KeyEvent;

public class MarketTest extends TestCase {
    public void test1(){
        assertEquals(4,checkThreadLive(new Market(2,2)));
    }
    public void test2(){
        assertEquals(8,checkThreadLive(new Market(4,4)));
    }
    public void test3(){
        assertEquals(12,checkThreadLive(new Market(4,8)));
    }
    public void test4(){
        assertEquals(6,checkThreadLive(new Market(0,6)));
    }

    //return count stopped threads
    private int checkThreadLive(Market m){
        m.openMarket();
        Thread robot = new Thread(new Rob());
        robot.start();
        while (true){
            try {
                Thread.sleep(1000);
            }catch (InterruptedException e){
                e.printStackTrace();
            }
            int cout = 0;
            for(Thread thread: m.getAllThreads()){
                if(!thread.isAlive()) cout++;
            }
            if(cout == m.getAllThreads().size()){
                return cout;
            }else {
                cout = 0;
            }
        }
    }
}

// imitation press key
class Rob implements Runnable{
    @Override
    public void run() {
            try {
                Thread.sleep(10000);
                new Robot().keyPress(KeyEvent.VK_A);
            }catch (InterruptedException | AWTException e){
                e.printStackTrace();
            }
    }
}
