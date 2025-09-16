
/**
* @brief Maclaurin series of function m! / (1 - x) ^ (m + 1), where m - integer parameter
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class m_fact_1mx_mp1_inverse_series : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
	m_fact_1mx_mp1_inverse_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series, m The integer constant parameter
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	m_fact_1mx_mp1_inverse_series(T x, K m);

	/**
	* @brief Computes the nth term of the Maclaurin series of  m! / (1 - x) ^ (m + 1)
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
private:

	/**
	* @brief The const parameter m of the series
	* @authors Pashkov B.B.
	*/
	const K m;

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
m_fact_1mx_mp1_inverse_series<T, K>::m_fact_1mx_mp1_inverse_series(T x, K m) : series_base<T, K>(x, static_cast<T>(this->fact(m)) / static_cast<T>(pow(static_cast<T>(1) - x, m + 1))), m(m), recurrent_series_base<T, K>(static_cast<T>(this->fact(m)))
{
	this->series_name = "m!/(1-x)^(m+1)";
	// Сходится при |x| < 1 (m-ая производная геометрического ряда)
	// Расходится при |x| ≥ 1

	if (!std::isfinite(series_base<T, K>::sum)) {
		throw std::overflow_error("sum is too big");
	}

	if (std::abs(this->x) >= 1 || !std::isfinite(x)) {
		series_base<T, K>::throw_domain_error("|x| must be < 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T m_fact_1mx_mp1_inverse_series<T, K>::access_row(K n)
{
	auto& series_vec = this->series_vector;
	auto old_size = series_vec.size();
	series_vec.reserve(n);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		series_vec.push_back(series_vec[i - 1] * this->x * static_cast<T>((this->m + static_cast<K>(i)) / static_cast<K>(i)));

	return series_vec[n];
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T m_fact_1mx_mp1_inverse_series<T, K>::operator()(K n) const
{
	return const_cast<m_fact_1mx_mp1_inverse_series<T, K>*>(this)->access_row(n);
}
