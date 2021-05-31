package Solution;

public class Consumer extends Thread{
    private int sleepTime = 0;
    private Market market;
    Consumer(int sleepTime, Market market){
        this.sleepTime = sleepTime;
        this.market = market;
    }
    private int count = 0;

    @Override
    public void run() {
        while (!market.getThreadFlag()){
            if(count == 1){
                try {
                    Thread.sleep(sleepTime);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }finally {
                    count = 0;
                }
            }
            try {
                //getting and deleting an item from a list
                market.getElement(0);
            }catch (IndexOutOfBoundsException e){
                count--;
            }
            count++;
        }
        System.out.print("Solution.Consumer закончил работу: ");
        System.out.println(currentThread().getId());
    }
}
