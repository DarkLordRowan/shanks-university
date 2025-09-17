#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series for function x / sqrt(1 - x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class x_div_1minx2_series final : public series_base<T, K>
{
public:
    x_div_1minx2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    x_div_1minx2_series(T x);

    /**
    * @brief Computes the nth term of the x / (1 - x) series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the x / sqrt(1 - x) series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

    constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return abs(x) >= static_cast<T>(1) || !isfinite(x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value)
			return abs(x) >= static_cast<float_precision>(1) || !isfinite(x);
		
		return false;
	}

};

template <Accepted T, std::unsigned_integral K>
x_div_1minx2_series<T, K>::x_div_1minx2_series(T x) : series_base<T, K>(x, x / (static_cast<T>(1) - x * x))
{
    this->series_name = "x/(1-x²)";
    // Сходится при |x| < 1 (биномиальный ряд в знаменателе)
    // Расходится при |x| ≥ 1

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T x_div_1minx2_series<T, K>::operator()(K n) const
{
    return pow(this->x, static_cast<T>(fma(2, n, 1))); // (101.1) [Rows.pdf]
}
