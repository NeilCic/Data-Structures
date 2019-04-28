package hashMap;

import java.util.AbstractCollection;
import java.util.AbstractSet;
import java.util.ArrayList;
import java.util.Collection;
import java.util.ConcurrentModificationException;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Objects;
import java.util.Set;
import java.util.stream.Stream;

import pair.Pair;

public class HashMap<K, V> implements Map<K, V> {

	/*-------------------- static variables --------------------*/

	private static final int DEFAULT_CAPACITY = 64;

	/*-------------------- instance variables ------------------*/

	private List<List<Entry<K, V>>> buckets;
	private final int capacity;
	private int modCount = 0;
	private Set<K> keySet;
	private Set<Entry<K, V>> entrySet;
	private Collection<V> valueCollection;

	/*------------------------API methods-----------------------*/

	public HashMap() {
		this(DEFAULT_CAPACITY);
	}

	public HashMap(int capacity) {
		if (capacity <= 0) { throw new IllegalArgumentException(); }

		this.capacity = capacity;
		buckets = new ArrayList<>();
		for (int i = 0; i < capacity; ++i) {
			buckets.add(new ArrayList<>());
		}
	}

	@Override
	public int size() {
		return buckets.stream().flatMap(list -> Stream.of(list)).filter(entry -> entry != null).mapToInt(List::size).sum();
	}

	@Override
	public boolean isEmpty() {
		for (int i = 0; i < capacity; ++i) {
			List<Entry<K, V>> list = buckets.get(i);
			if (!list.isEmpty()) {
				return false;
			}
		}

		return true;
	}

	@Override
	public V put(K key, V value) {
		Entry<K, V> newEntry = Pair.of(key, value);
		List<Entry<K, V>> list = buckets.get(hash(key));

		for (Entry<K, V> current : list) {
			if (Objects.equals(current.getKey(), key)) {
				V oldValue = current.getValue();
				current.setValue(value);

				return oldValue;
			}
		}

		++modCount;
		list.add(newEntry);

		return null;
	}

	@Override
	public boolean containsKey(Object key) { // do a common interface with put
		try {
			generalMethod(key, (entry, list) -> (entry.getValue()));
			return true;
		} catch (NoSuchElementException e) {
			return false;
		}
	}

	@Override
	public V remove(Object key) {
		try {
			return generalMethod(key, (entry, list) -> {
				V oldValue = entry.getValue();
				list.remove(list.indexOf(entry));
				++modCount;
				
				return oldValue;
			});
		} catch (NoSuchElementException e) {
			return null;
		}
	}

	@Override
	public V get(Object key) {
		try {
			return generalMethod(key, (entry, list) -> (entry.getValue()));
		} catch (NoSuchElementException e) {
			return null;
		}
	}

	private V generalMethod(Object key, Oper<K, V> op) {
		List<Entry<K, V>> list = buckets.get(hash(key));
		Oper<K, V> top = op;

		for (Entry<K, V> current : list) {
			if (Objects.equals(current.getKey(), key)) {

				return top.operation(current, list);
			}
		}

		throw new NoSuchElementException();
	}

	public interface Oper<K, V> {
		public V operation(Entry<K, V> entry, List<Entry<K, V>> list);
	}

	@Override
	public boolean containsValue(Object value) {
		for (int i = 0; i < capacity; ++i) {
			List<Entry<K, V>> list = buckets.get(i);
			for (Entry<K, V> entry : list) {
				if (Objects.equals(entry.getValue(), value)) {

					return true;
				}
			}
		}

		return false;
	}

	@Override
	public void putAll(Map<? extends K, ? extends V> m) {
		for (Entry<? extends K, ? extends V> entries : m.entrySet()) {
			put(entries.getKey(), entries.getValue());
		}

//		Alternate way:
//		Set<?> entries = m.entrySet();
//		for (Object current : entries) {
//			put(((Entry<? extends K, ? extends V>)current).getKey(), ((Entry<? extends K, ? extends V>)current).getValue());
//		}
	}

	@Override
	public void clear() {
		++modCount;
		buckets.stream().forEach(List::clear);
	}

	@Override
	public Set<K> keySet() {
		return (keySet == null ? keySet = new KeySet() : keySet);
	}

	@Override
	public Collection<V> values() {
		return (valueCollection == null ? valueCollection = new Values() : valueCollection);
	}

	@Override
	public Set<Entry<K, V>> entrySet() {
		return (entrySet == null ? entrySet = new EntrySet() : entrySet);
	}

	/*************************** private method *******************************/
	
	private int hash(Object key) {
		return Math.abs(Objects.hashCode(key) % capacity);
	}
	
	/*************************** private classes *******************************/
	private class EntrySet extends AbstractSet<Entry<K, V>> {
		@Override
		public Iterator<Entry<K, V>> iterator() {
			return new EntriesIterator();
		}

		@Override
		public int size() {
			return HashMap.this.size();
		}

		private class EntriesIterator implements Iterator<Entry<K, V>> {
			private int currentBucketIndex;
			private List<Entry<K, V>> currentInnerList;
			Entry<K, V> currentEntry;
			private final int iteratorVersion;

			// CTOR
			private EntriesIterator() {
				currentEntry = buckets.get(0).get(0);
				currentBucketIndex = 0;
				currentInnerList = buckets.get(currentBucketIndex);
				iteratorVersion = modCount;
			}

			@Override
			public boolean hasNext() {
				if (iteratorVersion != modCount) {
					throw new ConcurrentModificationException();
				}

				return currentEntry != null;
			}

			@Override
			public Entry<K, V> next() {
				if (iteratorVersion != modCount) {
					throw new ConcurrentModificationException();
				}

				if (!hasNext()) {
					throw new NoSuchElementException("[next method]> Next element doesn't exist");
				}

				Entry<K, V> oldEntry = currentEntry;
				int index = currentInnerList.indexOf(currentEntry);

				if (index + 1 < currentInnerList.size()) {
					currentEntry = currentInnerList.get(++index);
				} else {
					for (++currentBucketIndex; currentBucketIndex < capacity; ++currentBucketIndex) {
						currentInnerList = buckets.get(currentBucketIndex);
						if (!currentInnerList.isEmpty()) {
							currentEntry = currentInnerList.get(0);

							return oldEntry;
						}
					}
					
					currentEntry = null;
				}

				return oldEntry;
			}
		}
	}

	private class KeySet extends AbstractSet<K> {
		Set<Entry<K, V>> eSet = entrySet();

		@Override
		public Iterator<K> iterator() {
			return (eSet.iterator() != null ? new KeysIterator() : null);
		}

		@Override
		public int size() {
			return HashMap.this.size();
		}

		private class KeysIterator implements Iterator<K> {
			Iterator<Entry<K, V>> iter = eSet.iterator();

			@Override
			public boolean hasNext() {
				return iter.hasNext();
			}

			@Override
			public K next() {
				return iter.next().getKey();
			}
		}
	}

	private class Values extends AbstractCollection<V> {
		Collection<Entry<K, V>> eSet = entrySet();

		@Override
		public Iterator<V> iterator() {
			return (eSet.iterator() != null ? new ValuesIterator() : null);
		}

		@Override
		public int size() {
			return HashMap.this.size();
		}

		private class ValuesIterator implements Iterator<V> {
			Iterator<Entry<K, V>> iter = eSet.iterator();

			@Override
			public boolean hasNext() {
				return iter.hasNext();
			}

			@Override
			public V next() {
				return iter.next().getValue();
			}
		}
	}
}
