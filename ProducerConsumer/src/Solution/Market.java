package Solution;

import javax.swing.*;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.util.ArrayList;

public class Market {
    //number of producers and consumers
    private final int numberMan;
    private final int numberCon;

    public Market(int m, int c){
        numberMan = m;
        numberCon = c;
    }

    private boolean threadsStop = false;
    public boolean getThreadFlag(){
        return threadsStop;
    }
    public void setThreadFlag(){
        threadsStop = true;
    }

    private final ArrayList<Integer> arraInts = new ArrayList<Integer>();

    //add element to List
    public synchronized int getElement(int index){
        if(arraInts.size() != 0){
            int value = arraInts.get(index);
            arraInts.remove(index);
            return value;
        }
        throw  new IndexOutOfBoundsException();
    }
    //remove element from List
    public synchronized void setElement(int element){
        arraInts.add(element);
    }

    private final ArrayList<Thread> allThreads = new ArrayList<>();
    public ArrayList<Thread> getAllThreads(){
        return  allThreads;
    }
    //create and start Threads
    public void openMarket() {
        //creating a listener press key
        JFrame frame = new JFrame();
        frame.setVisible(true);
        frame.addKeyListener(new KeyAdapter() {
            public void keyPressed(KeyEvent e) {
                setThreadFlag();
            }
        });

        for(int i = 0; i < numberMan; i++){
            Producer m = new Producer((int) (Math.random()*10000), this);
            m.start();
            allThreads.add(m);
        }
        for(int i = 0; i < numberCon; i++){
           Consumer c = new Consumer((int) (Math.random()*10000),this);
           c.start();
           allThreads.add(c);
        }
    }
}
