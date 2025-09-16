#pragma once
#include "series/series_base.hpp"

/**
* @brief Maclaurin series of cosine function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class cos_series final : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
	cos_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	cos_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the cosine function
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the cosine functions
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
cos_series<T, K>::cos_series(T x) : series_base<T, K>(x, std::cos(x)), recurrent_series_base<T, K>(std::vector<T>{static_cast<T>(1), (static_cast<T>(-1) * x * x) / static_cast<T>(2)})
{
	this->series_name = "cos(x)";
	// Сходится при ∀x ∈ ℝ

	if (!std::isfinite(x)) {
		series_base<T, K>::throw_domain_error("x is not finite");
	}
}

template <std::floating_point T, std::unsigned_integral K>
T cos_series<T, K>::access_row(K n)
{
	auto& series_vec = this->series_vector; // ссылка на член базового класса
	auto old_size = series_vec.size();
	series_vec.reserve(n);

	T a = static_cast<T>(-this->x * this->x);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		series_vec.push_back(series_vec[i - 1] * (a / static_cast<T>(i * std::fma(4, i, -2))));

	return series_vec[n];
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T cos_series<T, K>::operator()(K n) const
{
	return const_cast<cos_series<T, K>*>(this)->access_row(n);
}
