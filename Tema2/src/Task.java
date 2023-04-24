import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

public class Task extends Thread {
    private int id;

    public static File f = new File("./order_products_out.txt");

    public Task(int id) {
        this.id = id;
    }

    public void run() {
        while(true) {
            boolean flag = true;
            synchronized("a") {
                if (!Tema2.myReader.hasNextLine()) {
                    flag = false;
                } else {
                    String data = Tema2.myReader.nextLine();
                    String commandId = data.split(",")[0];
                    int nrProducts = Integer.parseInt(data.split(",")[1]);

                    if (nrProducts == 0) {
                        continue;
                    }

                    AtomicInteger inQueue = new AtomicInteger(0);
                    ExecutorService tpe = Executors.newFixedThreadPool(Tema2.P);

                    for(int i = 0; i < nrProducts; i++) {
                        inQueue.incrementAndGet();
                    }

                    //File f = new File("./order_products_out.txt");
                    try {
                        FileWriter fw2 = new FileWriter(f, true);
                        for(int i = 0; i < nrProducts; i++) {
                            try {
                                tpe.submit(new MyRunnable(commandId, i, inQueue, tpe, fw2));
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }

                    try {
                        Tema2.myWriter.write(commandId + "," + nrProducts + ",shipped\n");
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }

            if (!flag) {
                break;
            }
        }
    }
}