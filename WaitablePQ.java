package waitablePQ;

import java.util.Comparator;
import java.util.PriorityQueue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

public class WaitablePQ<E> {
	private final PriorityQueue<E> pq;
	private final Object lockForPQ = new Object();
	private Semaphore sem = new Semaphore(0);

	public WaitablePQ() {
		pq = new PriorityQueue<>();
	}

	public WaitablePQ(Comparator<? super E> comparator) {
		pq = new PriorityQueue<>(comparator);
	}

	public boolean enqueue(E arg) {
		boolean isAdded = false;

		synchronized (lockForPQ) {
			isAdded = pq.add(arg);
		}

		sem.release();
		
		return isAdded;
	}

	public E dequeue() throws InterruptedException {
		E element = null;

		sem.acquire();

		synchronized (lockForPQ) {
			element = pq.poll();
		}

		return element;
	}

	public E dequeueWithTO(long time, TimeUnit unit) throws TimeoutException, InterruptedException { // missing timeout section
		E element = null;

		sem.acquire();

		synchronized (lockForPQ) {
			element = pq.poll();
		}

		return element;
	}
	
	public boolean isEmpty() {
		return pq.peek() == null;
	}
}
