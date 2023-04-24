import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Objects;
import java.util.Scanner;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnable implements Runnable {
    private final String command;
    private final int productNr;
    private int product_cnt = 0;
    private FileWriter fw2;

    AtomicInteger inQueue;
    ExecutorService tpe;

    File file = new File(Tema2.folder + "/order_products.txt");
    Scanner reader = new Scanner(file);

    public MyRunnable(String command, int productNr, AtomicInteger inQueue, ExecutorService tpe, FileWriter fw2) throws IOException {
        this.command = command;
        this.productNr = productNr;
        this.inQueue = inQueue;
        this.tpe = tpe;
        this.fw2 = fw2;
    }

    public void run() {
        while(reader.hasNextLine()) {
            String data = reader.nextLine();
            String commandId = data.split(",")[0];
            String productName = data.split(",")[1];

            if (!Objects.equals(commandId, command)) {
                continue;
            }

            product_cnt++;

            if (product_cnt == productNr + 1) {
                try {
                    //fw2.write(command + "," + productName + ",shipped\n");
                    FileWriter w = new FileWriter(new File("./order_products_out.txt"), true);
                    w.write(command + "," + productName + ",shipped\n");
                    w.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                break;
            }
        }

        int left = this.inQueue.decrementAndGet();
        if (left == 0) {
            try {
                fw2.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
            tpe.shutdown();
        }
    }
}