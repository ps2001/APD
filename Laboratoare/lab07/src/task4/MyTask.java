package task4;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.RecursiveTask;

public class MyTask extends RecursiveTask<Void> {
    ArrayList<Integer> partialPath;
    int destination;

    public MyTask(int destination, ArrayList<Integer> partialPath) {
        this.destination = destination;
        this.partialPath = partialPath;
    }

    @Override
    protected Void compute() {
        if (partialPath.get(partialPath.size() - 1) == destination) {
            System.out.println(partialPath);
            return null;
        }

        int lastNodeInPath = partialPath.get(partialPath.size() - 1);
        List<MyTask> tasks = new ArrayList<>();

        for (int[] ints : Main.graph) {
            if (ints[0] == lastNodeInPath) {
                if (partialPath.contains(ints[1]))
                    continue;
                ArrayList<Integer> newPartialPath = new ArrayList<>(partialPath);
                newPartialPath.add(ints[1]);
                MyTask t = new MyTask(destination, newPartialPath);
                tasks.add(t);
                t.fork();
            }
        }

        for (MyTask t : tasks) {
            t.join();
        }
        return null;
    }
}
