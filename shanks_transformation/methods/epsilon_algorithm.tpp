template <std::floating_point T, std::unsigned_integral K, typename series_templ>
epsilon_algorithm<T, K, series_templ>::epsilon_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T epsilon_algorithm<T, K, series_templ>::operator()(const K n, const K order) const
{

	using std::isfinite;

	if (order == static_cast<K>(0))
		return this->series->S_n(n);

	const K m = static_cast<K>(2) * order;
	K max_ind = m + n; // int -> K mark

	const K n1 = n - static_cast<K>(1);

	std::vector<T> e0(max_ind + static_cast<K>(1), static_cast<T>(0));
	std::vector<T> e1(max_ind                    , static_cast<T>(0));

	auto e0_add = &e0; // Pointer for vector swapping
	auto e1_add = &e1; // Pointer for vector swapping

	K j = max_ind;
	do {
		e0[j] = this->series->S_n(j);
	} while (--j > static_cast<K>(0));

	for (K i = static_cast<K>(0); i < m; ++i) {
		for (K j = n1; j < max_ind; ++j)
			(*e1_add)[j] += static_cast<T>(1) / ((*e0_add)[j + static_cast<K>(1)] - (*e0_add)[j]);

		--max_ind;
		std::swap(e0_add, e1_add);
		(*e1_add).erase((*e1_add).begin());
	}

	if (!isfinite((*e0_add)[n1]))
		throw std::overflow_error("division by zero");

	return (*e0_add)[n1];
}