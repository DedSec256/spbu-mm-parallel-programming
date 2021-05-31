package Solution;

public class Producer extends Thread{
    private int sleepTime = 0;
    private Market market;
    Producer(int sleepTime, Market market){
        this.sleepTime = sleepTime;
        this.market = market;
    }
    private int count = 0;
    @Override
    public void run() {
        while (!market.getThreadFlag()){
            if(count == 2){
                try {
                    Thread.sleep(sleepTime);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }finally {
                    count = 0;
                }
            }
            int element = (int)(Math.random()*10);
            //adding an element to a list
            market.setElement(element);
            count++;
        }
        System.out.print("Solution.Manufacturer закончил работу: ");
        System.out.println(currentThread().getId());
    }
}
