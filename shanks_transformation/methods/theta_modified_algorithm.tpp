template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T theta_modified_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	using std::isfinite;

	if (n < static_cast<K>(2)) return static_cast<T>(0);

	K m = n - static_cast<K>(1);

	K i1, i2;
	T delta, delta_next;

	std::vector<T> current(n);
	std::vector<T> next(n);

	for (K i = static_cast<K>(0); i < n; ++i)
		current[i] = this->series->operator()(i);
 
	for (K i = static_cast<K>(0); i < m; ++i) {

		i1 = i + static_cast<K>(1);
		i2 = i + static_cast<K>(2);

		delta = current[i1] - current[i];
		delta_next = (i2 < n) ? current[i2] - current[i1] : static_cast<T>(0);

		next[i] = current[i1];
		next[i]+= (delta * delta_next) / (delta - delta_next);

	}
	current = next;
	
	if (!isfinite(current[m - static_cast<K>(1)]))
		throw std::overflow_error("division by zero");
	return current[m - static_cast<K>(1)];
}