#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function 2 arcsin(x/2)^2
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class two_arcsin_square_x_halfed_series final : public series_base<T, K>
{
public:
	two_arcsin_square_x_halfed_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	two_arcsin_square_x_halfed_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

	constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return abs(x) > static_cast<T>(2) || !isfinite(x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value)
			return abs(x) > static_cast<float_precision>(2) || !isfinite(x);
		
		return false;
	}

};

template <Accepted T, std::unsigned_integral K>
two_arcsin_square_x_halfed_series<T, K>::two_arcsin_square_x_halfed_series(T x) : 
series_base<T, K>(
	x, 
	static_cast<T>(2) * asin(x * static_cast<T>(0.5)) * asin(x * static_cast<T>(0.5))
)
{
	this->series_name = "2*arcsin(x/2)^2";
	// Сходится при |x| ≤ 2 (ряд для arcsin(z) сходится при |z| ≤ 1, где z = x/2)
	// Расходится при |x| > 2

	if (domain_checker(x)) {
		this->throw_domain_error("|x| must be ≤ 2");
	}
}

template <Accepted T, std::unsigned_integral K>
constexpr T two_arcsin_square_x_halfed_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(fma(2, n, 1));
	const T fact_N = static_cast<T>(this->fact(n));
	return pow(this->x, static_cast<T>(a - 1)) * fact_N * fact_N / (static_cast<T>(a) * static_cast<T>(this->fact(a)));
}
