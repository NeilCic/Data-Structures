package pair;

import java.util.Comparator;
import java.util.Map.Entry;
import java.util.Objects;

/**
 * @param <K>
 * @param <V>
 * @description implementation of Map.Entry to be used in a HashMap
 */

public class Pair<K, V> implements Entry<K, V> {
	private final K key;
	private V value;
	
	//CTOR
	private Pair(K key, V value) {
		this.key = key;
		this.value = value;
	}
	
	@Override
	public K getKey() {
		return key;
	}

	@Override
	public V getValue() {
		return value;
	}

	@Override
	public V setValue(V value) {
		V oldValue = this.value;
		this.value = value;
		
		return oldValue;
	}

	@Override
	public boolean equals (Object obj) {
		if (!(obj instanceof Pair)) { return false; }

		Object userKey = ((Pair<?, ?>)obj).getKey();
		Object userVal = ((Pair<?, ?>)obj).getValue();
		
		return (Objects.equals(getKey(), userKey) && Objects.equals(getValue(), userVal));
	}
	
	@Override
	public int hashCode() {
		return Objects.hash(getKey(), getValue());
	}
	
	@Override
	public String toString() {
		return ("<" + key + "," + value + ">");
	}
	
	/**
	 * 
	 * @param pair
	 * @return a new pair with the original key being the value and 
	 * original value being the key of the new pair.
	 * @exception NullPointerException - if pair is null.
	 */
	public static <K, V> Pair<V, K> swap(Pair<K, V> pair){
		Objects.requireNonNull(pair);
		
		return Pair.of(pair.getValue(), pair.getKey());
	}
	
	public static <K, V> Pair<K, V> of(K key, V value){
		return new Pair<K,V>(key, value);
	}
	
	/**
	 * 
	 * @param arr - valid array of any type.
	 * @param comp - valid comparator.
	 * @return Pair<K,V> 
	 * K being the smallest element within the array and 
	 * V being the largest within the array
	 * @description this function uses the natural order of the objects
	 * (objects implement comparable)
	 */
	public static <T extends Comparable<? super T>> Pair<T,T> minMax(T[] arr){
		Objects.requireNonNull(arr, "Null received in minMax (Comparable version)");
		
		return minMaxImp(arr, (x,y) -> x.compareTo(y));
	}

	/**
	 * 
	 * @param arr - valid array of any type.
	 * @param comp - valid comparator.
	 * @return Pair<K,V> 
	 * K being the smallest element within the array and 
	 * V being the largest within the array
	 * @description this function uses a comparator
	 */
	public static <T> Pair<T,T> minMax (T[] arr, Comparator<? super T> comp) {
		Objects.requireNonNull(arr, "Null array received in minMax (Comparator version)");
		Objects.requireNonNull(comp, "Null comparator received in minMax (Comparator version)");
		
		return minMaxImp(arr, comp);
	}
	
	private static <T> Pair<T,T> minMaxImp(T[] arr, Comparator<? super T> comp) {
		T min = arr[0];
		T max = arr[0];
		
		for (int i = arr.length % 2; i < arr.length - 1; i += 2) {
			T tempMin;
			T tempMax;
			
			if (comp.compare(arr[i], arr[i + 1]) > 0) {
				tempMax = arr[i];
				tempMin = arr[i + 1];
			} else {
				tempMax = arr[i + 1];
				tempMin = arr[i];
			}
			
			if (comp.compare(tempMax, max) > 0) {
				max = tempMax;
			}
			
			if (comp.compare(tempMin, min) < 0) {
				min = tempMin;
			}
		}
		
		return Pair.of(min, max);
	}
}