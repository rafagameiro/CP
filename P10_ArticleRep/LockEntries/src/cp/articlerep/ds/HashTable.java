package cp.articlerep.ds;

import java.util.concurrent.locks.*;
import java.util.List;
import java.util.Iterator;
import java.util.Collections;

/**
 * @author Ricardo Dias
 */
public class HashTable<K extends Comparable<K>, V> implements Map<K, V> {

	private static class Node {
		public Object key;
		public Object value;
		public Node next;
                private ReentrantLock lock;

		public Node(Object key, Object value, Node next) {
			this.key = key;
			this.value = value;
			this.next = next;
                        this.lock = new ReentrantLock();
		}

                public void lock() {
                    lock.lock();
                }

                public void unlock() {
                    lock.unlock();
                }

	}

	private Node[] table;

	public HashTable() {
		this(1000);
	}

	public HashTable(int size) {
		this.table = new Node[size];
	}

	private int calcTablePos(K key) {
		return Math.abs(key.hashCode()) % this.table.length;
	}

	@SuppressWarnings("unchecked")
	@Override
	public V put(K key, V value) {
		int pos = this.calcTablePos(key);
		Node n = this.table[pos];

		while (n != null && !n.key.equals(key)) {
			n = n.next;
		}

		if (n != null) {
			V oldValue = (V) n.value;
			n.value = value;
			return oldValue;
		}

		Node nn = new Node(key, value, this.table[pos]);
		this.table[pos] = nn;

		return null;
	}

	@SuppressWarnings("unchecked")
	@Override
	public V remove(K key) {
		int pos = this.calcTablePos(key);
		Node p = this.table[pos];
		if (p == null) {
			return null;
		}

		if (p.key.equals(key)) {
			this.table[pos] = p.next;
			return (V) p.value;
		}

		Node n = p.next;
		while (n != null && !n.key.equals(key)) {
			p = n;
			n = n.next;
		}

		if (n == null) {
			return null;
		}

		p.next = n.next;

		return (V) n.value;
	}

	@SuppressWarnings("unchecked")
	@Override
	public V get(K key) {
		int pos = this.calcTablePos(key);
		Node n = this.table[pos];
		while (n != null && !n.key.equals(key)) {
			n = n.next;
		}
		return (V) (n != null ? n.value : null);
	}

	@SuppressWarnings("unchecked")
	@Override
	public void lockKey(K key) {
		int pos = this.calcTablePos(key);
		Node n = this.table[pos];
		while (n != null && !n.key.equals(key)) {
			n = n.next;
		}
                n.lock();
	}

	@SuppressWarnings("unchecked")
	@Override
	public void unlockKey(K key) {
		int pos = this.calcTablePos(key);
		Node n = this.table[pos];
		while (n != null && !n.key.equals(key)) {
			n = n.next;
		}
                n.unlock();
	}

	@SuppressWarnings("unchecked")
	@Override
	public void lockList(List list) {

            Collections.sort(list);
            Iterator it = list.iterator();
            while(it.hasNext())
                lockKey(it.next());

	}

	@SuppressWarnings("unchecked")
	@Override
	public void unlockList(List list) {

            Collections.sort(list);
            Iterator it = list.iterator();
            while(it.hasNext())
                unlockKey(it.next());

	}

	@Override
	public boolean contains(K key) {
		return get(key) != null;
	}

	/**
	 * No need to protect this method from concurrent interactions
	 */
	@Override
	public Iterator<V> values() {
		return new Iterator<V>() {

			private int pos = -1;
			private Node nextBucket = advanceToNextBucket();

			private Node advanceToNextBucket() {
				pos++;
				while (pos < HashTable.this.table.length
						&& HashTable.this.table[pos] == null) {
					pos++;
				}
				if (pos < HashTable.this.table.length)
					return HashTable.this.table[pos];

				return null;
			}

			@Override
			public boolean hasNext() {
				return nextBucket != null;
			}

			@SuppressWarnings("unchecked")
			@Override
			public V next() {
				V result = (V) nextBucket.value;

				nextBucket = nextBucket.next != null ? nextBucket.next
						: advanceToNextBucket();

				return result;
			}

		};
	}

	@Override
	public Iterator<K> keys() {
		return null;
	}

}
