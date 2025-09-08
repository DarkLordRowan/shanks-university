
template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T W_lubkin_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	if (order < static_cast<K>(0)) 
		throw std::domain_error("negative order input");

	return calculate(n, order, this->series->S_n(n), static_cast<K>(0));
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T W_lubkin_algorithm<T, K, series_templ>::calculate(K n, const K order, T S_n, const K j) const {

	using std::isfinite;
	using std::fma;

	/*
	* j - to fix n
	* S_n - partial sum of series.
	*/
	for (K i = static_cast<K>(0); i < j; ++i) 
		S_n += this->series->operator()(n  + i + static_cast<K>(1));
	
	n += j;

	if (order == static_cast<K>(0)) return S_n;

	//calculate all basic parts of transfor
	K order1 = order - static_cast<K>(1);
	T W0 = calculate(n, order1, S_n, static_cast<K>(0));
	T W1 = calculate(n, order1, S_n, static_cast<K>(1));
	T W2 = calculate(n, order1, S_n, static_cast<K>(2));
	T W3 = calculate(n, order1, S_n, static_cast<K>(3));
	
	//optimization calculations
	T Wo0 = W1 - W0;
	T Wo1 = W2 - W1;
	T Wo2 = W3 - W2;
	T Woo1 = Wo0 * (Wo2 - Wo1);
	T Woo2 = Wo2 * (Wo1 - Wo0);

	//T result = W1 - ((W2 - W1) * (W1 - W0) * (W3 - 2 * W2 + W1)) / ((W3 - W2) * (W2 - 2 * W1 + W0) - (W1 - W0) * (W3 - 2 * W2 + W1)); //straigh
	const T result = fma(-Wo1, Woo1 / (Woo2 - Woo1), W1); // optimized
	if (!isfinite(result))
		throw std::overflow_error("division by zero");
	return result;
}