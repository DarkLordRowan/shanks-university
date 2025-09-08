template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T ford_sidi_algorithm_two<T, K, series_templ>::operator()(const K n, const K k) const {

	using std::fma;
	using std::isfinite;

	if (n == static_cast<K>(0))
		throw std::domain_error("n = 0 in the input");

	T delta_squared_S_n;
	K m = n;

	do{
		
		delta_squared_S_n = this->series->S_n(m + static_cast<K>(2));
		delta_squared_S_n-= static_cast<T>(2) * this->series->S_n(m + static_cast<K>(1));
		delta_squared_S_n+= this->series->S_n(m);

	} while (delta_squared_S_n == 0 && --m > static_cast<K>(0));

	if (m == static_cast<K>(0))
		throw std::overflow_error("division by zero");

	T delta_S_n = this->series->S_n(m + static_cast<K>(1));
	delta_S_n -= this->series->S_n(m);

	const T T_n = fma(-delta_S_n, delta_S_n / delta_squared_S_n, this->series->S_n(m));
	
	if (!isfinite(T_n))
		throw std::overflow_error("division by zero");
	return T_n;
}