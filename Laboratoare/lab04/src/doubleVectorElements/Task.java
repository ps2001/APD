package doubleVectorElements;

public class Task extends Thread {
    private int id;

    public Task(int id) {
        this.id = id;
    }

    public void run() {
        int start = (int)((double)id * (double)Main.N / (double)Main.P);
        int end = Math.min((int)((double)(id + 1) * (double)Main.N / (double)Main.P), Main.N);
        for (int i = start; i < end; i++) {
            Main.v[i] *= 2;
        }
    }

}

/*
int start = id * N / P;
int end = min((id + 1)*N/P, N);
 */