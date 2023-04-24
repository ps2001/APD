package task2;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnable implements Runnable {
    AtomicInteger inQueue;
    ExecutorService tpe;

    int[]colors;
    int step;

    public MyRunnable(int[] colors, int step, AtomicInteger inQueue, ExecutorService tpe) {
        this.colors = colors;
        this.step = step;
        this.inQueue = inQueue;
        this.tpe = tpe;
    }

    private static boolean isEdge(int a, int b) {
        for (int[] ints : Main.graph) {
            if (ints[0] == a && ints[1] == b)
                return true;
        }
        return false;
    }

    private static boolean verifyColors(int[] colors, int step) {
        for (int i = 0; i < step; i++) {
            if (colors[i] == colors[step] && isEdge(i, step))
                return false;
        }
        return true;
    }

    static void printColors(int[] colors) {
        StringBuilder aux = new StringBuilder();
        for (int color : colors) {
            aux.append(color).append(" ");
        }
        System.out.println(aux);
    }

    int flag = 0;

    @Override
    public void run() {
        if (step == Main.N) {
            printColors(colors);
            flag = 1;
            int left = inQueue.decrementAndGet();
            if (left == 0) {
                tpe.shutdown();
                return;
            }
        }

        for (int i = 0; i < Main.COLORS; i++) {
            int[] newColors = colors.clone();
            newColors[step] = i;
            if (verifyColors(newColors, step)) {
                inQueue.incrementAndGet();
                tpe.submit(new MyRunnable(newColors, step + 1, inQueue, tpe));
            }
        }

        int left = inQueue.decrementAndGet();
        if (left == 0) {
            tpe.shutdown();
        }
    }
}
