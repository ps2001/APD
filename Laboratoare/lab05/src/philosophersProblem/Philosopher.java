package philosophersProblem;

public class Philosopher implements Runnable {
    private final Object leftFork;
    private final Object rightFork;
    private final int id;

    public Philosopher(int id, Object leftFork, Object rightFork) {
        this.leftFork = leftFork;
        this.rightFork = rightFork;
        this.id = id;
    }

    private void sleep() {
        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        if (id != Main.N - 1) {
            synchronized (rightFork) {
                sleep();
                synchronized (leftFork) {
                    System.out.println("Philosopher " + id + " is eating");
                }
            }
        } else {
            synchronized (leftFork) {
                sleep();
                synchronized (rightFork) {
                    System.out.println("Philosopher " + id + " is eating");
                }
            }
        }
    }
}
