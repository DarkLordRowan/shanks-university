#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of hyperbolic sine
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class sinh_series final : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
	sinh_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	sinh_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sinh function
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sinh functions
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

template <Accepted T, std::unsigned_integral K>
sinh_series<T, K>::sinh_series(T x) : series_base<T, K>(x, sinh(x)), recurrent_series_base<T, K>(x)
{
	this->series_name = "sinh(x)";
	// Сходится при ∀x ∈ ℝ

	if (!isfinite(x)) {
		series_base<T, K>::throw_domain_error("x is not finite");
	}
}

template <Accepted T, std::unsigned_integral K>
T sinh_series<T, K>::access_row(K n)
{
	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	T a = this->x * this->x;

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i)
		this->series_vector.push_back(this->series_vector[i - 1] * a / static_cast<T>(i * fma(4, i, 2)));

	return this->series_vector[n];
}

template <Accepted T, std::unsigned_integral K>
constexpr T sinh_series<T, K>::operator()(K n) const
{
	return const_cast<sinh_series<T, K>*>(this)->access_row(n);
}
