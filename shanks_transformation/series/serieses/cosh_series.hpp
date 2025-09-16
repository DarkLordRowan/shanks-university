#pragma once
#include "series/series_base.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class cosh_series final : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
	cosh_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	cosh_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of hyperbolic cosine
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series
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
cosh_series<T, K>::cosh_series(T x) : series_base<T, K>(x, std::cosh(x)), recurrent_series_base<T, K>(std::vector<T>{static_cast<T>(1), (x * x) / static_cast<T>(2)})
{
	this->series_name = "cosh(x)";
	// Сходится при ∀x ∈ ℝ

	if (!std::isfinite(x)) {
		series_base<T, K>::throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T cosh_series<T, K>::access_row(K n)
{
	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	T a = static_cast<T>(this->x * this->x);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		this->series_vector.push_back(this->series_vector[i - 1] * (a / static_cast<T>(i * std::fma(4, i, -2)))); // (4.2) [Rows.pdf]

	return this->series_vector[n];
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T cosh_series<T, K>::operator()(K n) const
{
	return const_cast<cosh_series<T, K>*>(this)->access_row(n);
}
