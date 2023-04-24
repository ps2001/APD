package parallelTree;

import java.util.concurrent.CyclicBarrier;

public class Main {
	public static CyclicBarrier barrier = new CyclicBarrier(3);
	public static int N_ITERATIONS = 100;

	public static void main(String[] args) {
		Thread[] threads = new Thread[3];
		System.out.println("Parallel tree problem");
		
		for (int j = 0; j < N_ITERATIONS; j++) {
			TreeNode tree = new TreeNode(1);
			threads[0] = new Thread(new ReadTreePart(tree, "C:\\Users\\Silviu\\Documents\\APD\\laboratoare\\lab06\\treePart1.txt"));
			threads[1] = new Thread(new ReadTreePart(tree, "C:\\Users\\Silviu\\Documents\\APD\\laboratoare\\lab06\\treePart2.txt"));
			threads[2] = new Thread(new VerifyTree(tree));
			for (int i = 0; i < 3; i++) {
				threads[i].start();
			}

			for (int i = 0; i < 3; i++) {
				try {
					threads[i].join();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
	}
}