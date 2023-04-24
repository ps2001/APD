package oneProducerOneConsumer;

public class Buffer {
    private int a = -1;
    private boolean empty = true;

    void put(int value) throws InterruptedException {
        synchronized (this) {
            while (!empty) {
                this.wait();
            }

            a = value;
            empty = false;
            this.notifyAll();
        }
    }

    int get() throws InterruptedException {
        synchronized (this) {
            while (empty) {
                this.wait();
            }

            int value = a;
            empty = true;
            this.notifyAll();
            return value;
        }
    }
}