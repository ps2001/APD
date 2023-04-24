package shortestPathsFloyd_Warshall;

public class Task extends Thread {
    private int id;

    public Task(int id) {
        this.id = id;
    }

    public void run() {
        int start = (int)(id * (double)Main.N / Main.P);
        int end = Math.min((int)((id + 1) * (double)Main.N / Main.P), Main.N);

        for (int k = 0; k < 5; k++) {
            for (int i = start; i < end; i++) {
                for (int j = 0; j < 5; j++) {
                    Main.graph2[i][j] = Math.min(Main.graph2[i][j], Main.graph2[i][k] + Main.graph2[k][j]);
                }
            }
        }
    }
}
