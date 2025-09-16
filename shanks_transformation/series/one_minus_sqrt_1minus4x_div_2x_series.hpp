
/**
* @brief Series of function (1 - sqrt(1 - 4x)) / 2x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class one_minus_sqrt_1minus4x_div_2x_series : public series_base<T, K>
{
public:
	one_minus_sqrt_1minus4x_div_2x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	one_minus_sqrt_1minus4x_div_2x_series(T x);

	/**
	* @brief Computes the nth term of the series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
one_minus_sqrt_1minus4x_div_2x_series<T, K>::one_minus_sqrt_1minus4x_div_2x_series(T x) : series_base<T, K>(x, (static_cast<T>(1) - static_cast<T>(sqrt(std::fma(-4, x, 1)))) / (static_cast<T>(2) * x))
{
	this->series_name = "(1-sqrt(1-4x))/(2x)";
	// Сходится при 0 < |x| ≤ 0.25 (производящая функция для чисел Каталана)
	// Расходится при |x| > 0.25 или x = 0

	if (std::abs(x) > static_cast<T>(0.25) || x == 0 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be in (0, 0.25]");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_minus_sqrt_1minus4x_div_2x_series<T, K>::operator()(K n) const
{
	const T xn = static_cast<T>(std::pow(this->x, n));
	const T binom_coef = static_cast<T>(this->binomial_coefficient(static_cast<T>(0.5), n + static_cast<K>(1)));
	const T power_2 = this->minus_one_raised_to_power_n(n) * static_cast<T>(std::pow(2, std::fma(2, n, 1)));

	return power_2 * binom_coef * xn; // (54.1) [Rows.pdf]
}
