#pragma once
#include "series.h"

/**
* @brief Maclaurin series of function 0.5 * sqrt(pi) * erf(x)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class erf_series : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
	erf_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	erf_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of sqrt(pi) * erf(x) / 2
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
erf_series<T, K>::erf_series(T x) : series_base<T, K>(x, static_cast<T>(0.5) * static_cast<T>(std::sqrt(std::numbers::pi)) * static_cast<T>(std::erf(x))), recurrent_series_base<T, K>(x)
{
	this->series_name = "√π/2 * erf(x)";
	// Сходится при ∀x ∈ ℝ (ряд для erf(x) сходится на всей числовой прямой)

	if (!std::isfinite(x)) {
		series_base<T, K>::throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T erf_series<T, K>::access_row(K n)
{
	auto& series_vec = this->series_vector; // ссылка на член базового класса
	auto old_size = series_vec.size();
	series_vec.reserve(n);
	T a;
	T b = static_cast<T>(-this->x * this->x);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
	{
		a = static_cast<T>(std::fma(2, i, 1));
		series_vec.push_back(series_vec[i - 1] * (b * (a - static_cast<T>(2)) / (static_cast<T>(i) * a)));
	}
	return series_vec[n];
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T erf_series<T, K>::operator()(K n) const
{
	return const_cast<erf_series<T, K>*>(this)->access_row(n);
}
