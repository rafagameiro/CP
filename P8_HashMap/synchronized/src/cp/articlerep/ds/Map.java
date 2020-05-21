package cp.articlerep.ds;

/**
 * @author Ricardo Dias
 */
public interface Map<K, V> {
        public int size();
	public V put(K key, V value);
	public V remove(K key);
	public V get(K key);
        public boolean validate(K key);

}
