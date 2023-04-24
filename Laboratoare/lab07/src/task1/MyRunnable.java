package task1;

import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnable implements Runnable {
    ArrayList<Integer> partialPath;
    ExecutorService tpe;
    AtomicInteger inQueue;
    int destination;


    public MyRunnable(int destination, ArrayList<Integer> partialPath, ExecutorService tpe, AtomicInteger inQueue) {
        this.partialPath = partialPath;
        this.tpe = tpe;
        this.inQueue = inQueue;
        this.destination = destination;
    }

    @Override
    public void run() {
        if (partialPath.get(partialPath.size() - 1) == destination) {
            System.out.println(partialPath);
        }

        int lastNodeInPath = partialPath.get(partialPath.size() - 1);
        for (int[] ints : Main.graph) {
            if (ints[0] == lastNodeInPath) {
                if (partialPath.contains(ints[1]))
                    continue;
                inQueue.incrementAndGet();
                ArrayList<Integer> newPartialPath = new ArrayList<>(partialPath);
                newPartialPath.add(ints[1]);
                tpe.submit(new MyRunnable(destination, newPartialPath, tpe, inQueue));
            }
        }

        int left = inQueue.decrementAndGet();
        if (left == 0) {
            tpe.shutdown();
        }
    }
}
