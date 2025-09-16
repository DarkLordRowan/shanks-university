
/**
* @brief Fourier series of system functions -x - π/4, -pi < x <= 0
*											 -π/4,   0 < x < pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class minus_x_minus_pi_4_or_minus_pi_4_series : public series_base<T, K>
{
public:
	minus_x_minus_pi_4_or_minus_pi_4_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	minus_x_minus_pi_4_or_minus_pi_4_series(T x);

	/**
	* @brief Computes the nth term of the Fourier series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Fourier series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
minus_x_minus_pi_4_or_minus_pi_4_series<T, K>::minus_x_minus_pi_4_or_minus_pi_4_series(T x)
: series_base<T, K>(x,
	x<=0 && x<static_cast<T>(std::numbers::pi) ? static_cast<T>(-0.25 * std::numbers::pi) :
	x>0  && x<static_cast<T>(std::numbers::pi) ? static_cast<T>(-0.25 * std::numbers::pi) - x :
	x
)
{
	this->series_name = "f(x) = { -x - π/4, -π < x < 0; -π/4, 0 ≤ x < π }";
	// Сходится при -π < x < π (ряд Фурье для кусочно-линейной функции)
	// Расходится при x ≤ -π или x ≥ π

	if (abs(x) >= static_cast<T>(std::numbers::pi) || !std::isfinite(x)) {
		this->throw_domain_error("x must be in (-π, π)");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_x_minus_pi_4_or_minus_pi_4_series<T, K>::operator()(K n) const
{
	if (n == 0)
		return static_cast<T>(-std::numbers::pi * 0.5);

	const T a_n = static_cast<T>(2) * static_cast<T>(n % 2) / static_cast<T>(std::numbers::pi) / static_cast<T>(n * n);
	const T b_n = this->minus_one_raised_to_power_n(n) / static_cast<T>(n);
	return a_n * std::cos(static_cast<T>(n) * this->x) + b_n * std::sin(static_cast<T>(n) * this->x); // (62.5) [Rows.pdf], (1 - 2 * ((n + 1) & 1)) = (-1)^{n+1}
}
