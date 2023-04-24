package multipleProducersMultipleConsumers;

import java.util.concurrent.ArrayBlockingQueue;

public class Buffer {
	int value;
	private ArrayBlockingQueue<Integer> values = new ArrayBlockingQueue<>(1);

	void put(int value) throws InterruptedException {
		values.put(value);
	}

	int get() throws InterruptedException {
		return values.take();
	}
}
