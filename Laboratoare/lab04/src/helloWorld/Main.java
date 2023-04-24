package helloWorld;

public class Main {
    public static void main(String[] args) throws InterruptedException {
        int num_of_threads = 4;
        Thread[] threads = new Thread[num_of_threads];

        for (int i = 0; i < num_of_threads; i++) {
            threads[i] = new Task(i);
            threads[i].start();
        }

        for (int i = 0; i < num_of_threads; i++) {
            threads[i].join();
        }
    }
}
