package multipleProducersMultipleConsumersNBuffer;

import java.util.Queue;

public class Buffer {
    
    Queue<Integer> queue;
    private int sz = 0;
    private boolean full = false;
    private boolean empty = true;
    private int limit;
    
    public Buffer(int size) {
        queue = new LimitedQueue<>(size);
        limit = size;
    }

    // producator
	public void put(int value) throws InterruptedException {
        synchronized (this) {
            while (full) {
                this.wait();
            }

            queue.add(value);
            empty = false;
            if (queue.size() == limit) {
                full = true;
            }
            this.notifyAll();
        }
	}

    // consumator
	public int get() throws InterruptedException {
        synchronized (this) {
            while (empty) {
                this.wait();
            }

            Integer result = queue.poll();

            full = false;
            empty = queue.isEmpty();
            this.notifyAll();
            return result;
        }
	}
}
