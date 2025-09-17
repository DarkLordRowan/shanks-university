#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function 0.5 * asin(2x), where asin(x) is inverse sine of x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class half_asin_two_x_series final : public series_base<T, K>
{
public:
    half_asin_two_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    half_asin_two_x_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of 0.5 * asin(2x)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

    constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return abs(x) > static_cast<T>(0.5) || !isfinite(x); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(x.real()) > static_cast<float_precision>(0.5) || !isfinite(x); 
		
		return false;

	}
};

template <Accepted T, std::unsigned_integral K>
half_asin_two_x_series<T, K>::half_asin_two_x_series(T x) : series_base<T, K>(x, static_cast<T>(0.5) * asin(static_cast<T>(2) * x))
{
    this->series_name = "0.5*asin(2x)";
    // Сходится при |x| ≤ 0.5 (абсолютно при |x| < 0.5, условно при |x| = 0.5)

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be ≤ 0.5");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T half_asin_two_x_series<T, K>::operator()(K n) const
{
    const T _fact_n = static_cast<T>(this->fact(n));
    const T a = static_cast<T>(fma(2,n,1));
    return static_cast<T>(this->fact(2 * n)) * pow(this->x, a) / (_fact_n * _fact_n * a); // (12.1) [Rows.pdf]
}
