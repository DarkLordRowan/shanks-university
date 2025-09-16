
/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class exp_series : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
	exp_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	exp_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the exponent
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the exponent
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;


private:
	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	T access_row(K n);
};

template <std::floating_point T, std::unsigned_integral K>
exp_series<T, K>::exp_series(T x) : series_base<T, K>(x, std::exp(x)), recurrent_series_base<T, K>(static_cast<T>(1))
{
	this->series_name = "exp(x)";
	// Сходится при ∀x ∈ ℝ

	if (!std::isfinite(x)) {
		series_base<T, K>::throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T exp_series<T, K>::access_row(K n)
{
	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		this->series_vector.push_back(this->series_vector[i - 1] * this->x / i);

	return this->series_vector[n];
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T exp_series<T, K>::operator()(K n) const
{
	return const_cast<exp_series<T, K>*>(this)->access_row(n);
}
