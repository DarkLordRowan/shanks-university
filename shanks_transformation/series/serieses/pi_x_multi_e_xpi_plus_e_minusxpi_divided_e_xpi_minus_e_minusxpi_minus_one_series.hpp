#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function pi * x * (e^(pi*x) + e^(-pi*x)) / (e^(pi*x) - e^(-pi*x)) - 1
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series final : public series_base<T, K>
{
public:
	pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series<T, K>::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series(T x) : 
series_base<T, K>(
	x, 
	static_cast<T>(std::numbers::pi) * x / tanh(static_cast<T>(std::numbers::pi) * x) - static_cast<T>(1)
)
{
	this->series_name = "π*x*coth(π*x)-1";
	// Сходится при ∀x ∈ ℝ\{0} (ряд для гиперболического котангенса)
	// При x = 0 требуется специальное рассмотрение (предел)

	if (x == static_cast<T>(0) || !isfinite(x)) {
		this->throw_domain_error("x must be finite and non-zero");
	}
}

template <Accepted T, std::unsigned_integral K>
constexpr T pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_minus_one_series<T, K>::operator()(K n) const
{
	const T a = static_cast<T>(n + 1);
	const T b = this->x * this->x;
	return static_cast<T>(2) * b / (b + a * a); // (45.4) [Rows.pdf]
}
