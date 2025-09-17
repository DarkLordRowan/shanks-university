#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function (1 - 4x) ^ (-1/2)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class inverse_sqrt_1m4x_series final : public series_base<T, K>
{
public:
    inverse_sqrt_1m4x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    inverse_sqrt_1m4x_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of (1 - 4x) ^ (-1/2)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

    constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return abs(x) >= static_cast<T>(0.25) || !isfinite(x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(x) >= static_cast<float_precision>(0.25) || !isfinite(x);

		return false;
	}
};

template <Accepted T, std::unsigned_integral K>
inverse_sqrt_1m4x_series<T, K>::inverse_sqrt_1m4x_series(T x) : 
series_base<T, K>(
    x,
     pow(fma(static_cast<T>(-4), x, static_cast<T>(1)), static_cast<T>(-0.5))
)
{
    this->series_name = "1/sqrt(1-4x)";
    // Сходится при |x| < 0.25 (биномиальный ряд)
    // Расходится при |x| ≥ 0.25

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be < 0.25");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T inverse_sqrt_1m4x_series<T, K>::operator()(K n) const
{
    const T _fact_n = static_cast<T>(this->fact(n));
    return static_cast<T>(this->fact(2 * n)) * pow(this->x, static_cast<T>(n)) / (_fact_n * _fact_n); // (17.2) [Rows.pdf]
}
