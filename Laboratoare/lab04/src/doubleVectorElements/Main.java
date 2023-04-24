package doubleVectorElements;

public class Main {
    static int P = 4;
    static int N = 100000013;
    static int[] v = new int[N];

    public static void main(String[] args) {

        for (int i = 0; i < N; i++) {
            v[i] = i;
        }

        // Parallelize me using P threads
//        for (int i = 0; i < N; i++) {
//            v[i] = v[i] * 2;
//        }

        Thread[] t  = new Thread[4];

        for (int i = 0; i < P; i++) {
            t[i] = new Task(i);
            t[i].start();
        }

        for (int i = 0; i < P; ++i) {
            try {
                t[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        for (int i = 0; i < N; i++) {
            if (v[i] != i * 2) {
                System.out.println("Wrong answer");
                System.exit(1);
            }
        }
        System.out.println("Correct");
    }

}
