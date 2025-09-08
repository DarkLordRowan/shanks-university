template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T epsilon_modified_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	using std::isfinite;

	std::vector<std::vector<T>> eps(n, std::vector<T>(n, static_cast<T>(0)));

	for (K i = static_cast<K>(0); i < n; ++i)
		eps[i][0] = this->series->operator()(i);

	K m1, k1;

	for (K k = static_cast<K>(1); k < n; ++k)
	
		for (K m = k; m < n; ++m) {
			m1 = m - static_cast<K>(1);
			k1 = k - static_cast<K>(1);
			eps[m][k] = eps[m1][k1];
			eps[m][k] += static_cast<T>(1) / (eps[m][k1] - eps[m1][k1]) / (
				(
					k & static_cast<K>(1)) ? 
					static_cast<T>(order) + static_cast<T>(k + static_cast<K>(1)) / static_cast<T>(2) 
					: 
					static_cast<T>(2) / static_cast<T>(k + static_cast<K>(2)
				)
			);

		}

	if (!isfinite(eps[n - static_cast<K>(1)][n - static_cast<K>(1)]))
		throw std::overflow_error("division by zero");

	return eps[n - static_cast<K>(1)][n - static_cast<K>(1)];
}