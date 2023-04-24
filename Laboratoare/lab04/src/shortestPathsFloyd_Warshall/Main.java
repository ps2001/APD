package shortestPathsFloyd_Warshall;

public class Main {
    static int P = Runtime.getRuntime().availableProcessors();
    static int M = 9;
    static int N = 5;

    static int[][] graph = {{0, 1, M, M, M},
            {1, 0, 1, M, M},
            {M, 1, 0, 1, 1},
            {M, M, 1, 0, M},
            {M, M, 1, M, 0}};

    static int[][] graph2 = {{0, 1, M, M, M},
            {1, 0, 1, M, M},
            {M, 1, 0, 1, 1},
            {M, M, 1, 0, M},
            {M, M, 1, M, 0}};

    public static void main(String[] args) throws InterruptedException {

        // Parallelize me (You might want to keep the original code in order to compare)
        for (int k = 0; k < 5; k++) {
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    graph[i][j] = Math.min(graph[i][k] + graph[k][j], graph[i][j]);
                }
            }
        }

        Thread[] t = new Thread[P];

        for (int i = 0; i < P; i++) {
            t[i] = new Task(i);
            t[i].start();
        }

        for (int i = 0; i < P; i++) {
            t[i].join();
        }

        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                System.out.print(graph[i][j] + " ");
            }
            System.out.println();
        }

        System.out.println();

        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                System.out.print(graph2[i][j] + " ");
            }
            System.out.println();
        }
    }
}
