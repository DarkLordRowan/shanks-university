#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function (sinh(x) + sin(x)) / 2
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class mean_sinh_sin_series final : public series_base<T, K>, public recurrent_series_base<T, K>
{
public:
	mean_sinh_sin_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Pashkov B.B.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	mean_sinh_sin_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of (sinh(x) + sin(x)) / 2
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

template <Accepted T, std::unsigned_integral K>
mean_sinh_sin_series<T, K>::mean_sinh_sin_series(T x) : series_base<T, K>(x, static_cast<T>(0.5) * (sinh(x) + sin(x))), recurrent_series_base<T, K>(x)
{
	this->series_name = "0.5*(sinh(x) + sin(x))";
	// Сходится при ∀x ∈ ℝ (как сумма двух всюду сходящихся рядов)

	if (!isfinite(x)) {
		series_base<T, K>::throw_domain_error("x is not finite");
	}
}

template <Accepted T, std::unsigned_integral K>
T mean_sinh_sin_series<T, K>::access_row(K n)
{
	auto& series_vec = this->series_vector; // ссылка на член базового класса
	auto old_size = series_vec.size();
	series_vec.reserve(n);
	T a;
	for (auto i = old_size; i <= static_cast<typename std::vector<T>::size_type>(n); ++i) {
		a = static_cast<T>(fma(4, i, 1));
		series_vec.push_back(series_vec[i - 1] * pow(this->x, static_cast<T>(4)) / static_cast<T>((4 * i * a * (a - 2) * (a - 3))));
	}
	return series_vec[n];
}

template <Accepted T, std::unsigned_integral K>
constexpr T mean_sinh_sin_series<T, K>::operator()(K n) const
{
	return const_cast<mean_sinh_sin_series<T, K>*>(this)->access_row(n);
}
