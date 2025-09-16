
/**
* @brief function for testing new requrrive serieses or converting basic to
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class recurrent_testing_series : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
	recurrent_testing_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	*/
	recurrent_testing_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

private:
	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the series
	*/
	T access_row(K n);
};

template <std::floating_point T, std::unsigned_integral K>
recurrent_testing_series<T, K>::recurrent_testing_series(T x) : series_base<T, K>(x, 0), recurrent_series_base<T, K>(x)
{
	this->series_name = "recurrent_testing_series";
	// Область сходимости зависит от конкретной тестовой рекуррентной функции
	// Требуется уточнение для конкретного тестового случая

	if (!std::isfinite(x)) {
		this->throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T recurrent_testing_series<T, K>::access_row(K n)
{
	auto& series_vec = this->get_series_vector();
	K old_size = series_vec.size();
	series_vec.reserve(n);

	for (K i = old_size; i <= n; ++i)
		series_vec.push_back(series_vec[i - 1] * (this->x * this->x) / (i * std::fma(4, i, 2)));

	return series_vec[n];
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T recurrent_testing_series<T, K>::operator()(K n) const
{
	return const_cast<recurrent_testing_series<T, K>*>(this)->access_row(n);
}