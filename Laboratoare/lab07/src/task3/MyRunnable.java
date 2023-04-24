package task3;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnable implements Runnable {
    AtomicInteger inQueue;
    ExecutorService tpe;

    int[] graph;
    int step;

    public MyRunnable(int step, int[] graph, AtomicInteger inQueue, ExecutorService tpe) {
        this.step = step;
        this.graph = graph;
        this.inQueue = inQueue;
        this.tpe = tpe;
    }

    private static boolean check(int[] arr, int step) {
        for (int i = 0; i <= step; i++) {
            for (int j = i + 1; j <= step; j++) {
                if (arr[i] == arr[j] || arr[i] + i == arr[j] + j || arr[i] + j == arr[j] + i)
                    return false;
            }
        }
        return true;
    }

    private static void printQueens(int[] sol) {
        StringBuilder aux = new StringBuilder();
        for (int i = 0; i < sol.length; i++) {
            aux.append("(").append(sol[i] + 1).append(", ").append(i + 1).append("), ");
        }
        aux = new StringBuilder(aux.substring(0, aux.length() - 2));
        System.out.println("[" + aux + "]");
    }

    @Override
    public void run() {
        if (Main.N == step) {
            printQueens(graph);
            int left = inQueue.decrementAndGet();
            if (left == 0) {
                tpe.shutdown();
                return;
            }
        }

        for (int i = 0; i < Main.N; ++i) {
            int[] newGraph = graph.clone();
            newGraph[step] = i;

            if (check(newGraph, step)) {
                inQueue.incrementAndGet();
                tpe.submit(new MyRunnable(step + 1, newGraph, inQueue, tpe));
            }
        }

        int left = inQueue.decrementAndGet();
        if (left == 0) {
            tpe.shutdown();
        }
    }
}
