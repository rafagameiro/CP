package cp.articlerep.ds;

import java.util.List;
import java.util.Iterator;

/**
 * @author Ricardo Dias
 */
public interface Map<K extends Comparable<K>, V> {
	public V put(K key, V value);
	public boolean contains(K key);
	public V remove(K key);
	public V get(K key);
        public void lockKey(K key);
        public void unlockKey(K key);
        public void lockList(List list);
        public void unlockList(List list);


	public Iterator<V> values();
	public Iterator<K> keys();
}
