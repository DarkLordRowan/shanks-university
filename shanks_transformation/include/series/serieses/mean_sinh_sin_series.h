
/**
* @brief Maclaurin series of (sinh(x) + sin(x)) / 2
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>
#include <vector>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class mean_sinh_sin_series final : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
	mean_sinh_sin_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	*/
	explicit mean_sinh_sin_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of (sinh(x) + sin(x)) / 2
	* @authors Pashkov B.B.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] T operator()(K n) const override;

private:
	/**
	* @brief Computes nth term of the series
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @return nth term of the series
	*/
	T acsess_row(K n) override;
};

template <typename T, typename K>
mean_sinh_sin_series<T, K>::mean_sinh_sin_series(T x) : series_base<T, K>(x, static_cast<T>(0.5 * (std::sinh(x) + std::sin(x)))), recurrent_series_base<T, K>(x) {}

template <typename T, typename K>
T mean_sinh_sin_series<T, K>::acsess_row(K n)
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	auto old_size = this->series_vector.size();
	this->series_vector.reserve(n);

	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) {
		this->series_vector.push_back(this->series_vector[i - 1] * static_cast<T>(std::pow(this->x, 4) / ((4 * i + 1) * (4 * i) * (4 * i - 1) * (4 * i - 2))));
	}

	return this->series_vector[n];
} //std::pow(this->x, 4 * n + 1) / this->fact(4 * n + 1); FIX THIS SHIT!

template <typename T, typename K>
T mean_sinh_sin_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	const T a = const_cast<mean_sinh_sin_series<T, K>*>(this)->acsess_row(n);
	return a;
}

