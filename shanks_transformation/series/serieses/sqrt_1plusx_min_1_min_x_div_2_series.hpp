#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function sqrt(1 + x) - 1 - x/2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class sqrt_1plusx_min_1_min_x_div_2_series final : public series_base<T, K>
{
public:
    sqrt_1plusx_min_1_min_x_div_2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    sqrt_1plusx_min_1_min_x_div_2_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

    constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return abs(x) > static_cast<T>(1) || !isfinite(x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value)
			return abs(x) > static_cast<float_precision>(1) || !isfinite(x);
		
		return false;
	}
};

template <Accepted T, std::unsigned_integral K>
sqrt_1plusx_min_1_min_x_div_2_series<T, K>::sqrt_1plusx_min_1_min_x_div_2_series(T x) : 
series_base<T, K>(
    x, 
    sqrt(static_cast<T>(1) + x) - static_cast<T>(1) - static_cast<T>(0.5) * x
)
{
    this->series_name = "√(1+x)-1-x/2";
    // Сходится при |x| ≤ 1 (биномиальный ряд, абсолютно при |x| < 1, условно при x = -1)
    // Расходится при |x| > 1

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T sqrt_1plusx_min_1_min_x_div_2_series<T, K>::operator()(K n) const
{
    return this->binomial_coefficient(static_cast<T>(0.5), n + 2) * pow(this->x, static_cast<T>(n + 2)); // (93.2) [Rows.pdf]
}
