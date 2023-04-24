import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

public class Tema2 {
    public static String folder;
    public static int P;

    public static String in_file1;
    public static File myObj;
    public static Scanner myReader;
    public static FileWriter myWriter;

    public static void main(String[] args) throws InterruptedException, IOException {
        folder = args[0];
        P = Integer.parseInt(args[1]);

        in_file1 = folder + "/orders.txt";
        myObj = new File(in_file1);
        myReader = new Scanner(myObj);

        if (Task.f.exists()) {
            Task.f.delete();
        }

        File out_file = new File("./orders_out.txt");

        if (out_file.exists()) {
            out_file.delete();
        }

        myWriter = new FileWriter(out_file, true);

        Thread[] t = new Thread[P];
        for(int i = 0; i < P; i++) {
            t[i] = new Task(i);
            t[i].start();
        }

        for(int i = 0; i < P; i++) {
            t[i].join();
        }

        myReader.close();
        myWriter.close();
    }
}