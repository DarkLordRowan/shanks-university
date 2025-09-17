#pragma once
#include "../series_base.hpp"

/**
* @brief Trigonometric series of function 1/12 * (3x^2 - pi^2)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class one_twelfth_3x2_pi2_series final : public series_base<T, K>
{
public:
    one_twelfth_3x2_pi2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    one_twelfth_3x2_pi2_series(T x);

    /**
    * @brief Computes the nth term of the Trigonometric series of 1/12 * (3x^2 - pi^2)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

    constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value)
			return abs(x) > static_cast<T>(PI) || !isfinite(x);
        
        if constexpr ( std::is_same<T, float_precision>::value || std::is_same<T, complex_precision<float_precision>>::value)
			return abs(x) > arbPI || !isfinite(x);
		
		return false;
	}

};

template <Accepted T, std::unsigned_integral K>
one_twelfth_3x2_pi2_series<T, K>::one_twelfth_3x2_pi2_series(T x) : 
series_base<T, K>(
    x, 
    fma(static_cast<T>(3) * x, x, static_cast<T>(-std::numbers::pi) * static_cast<T>(std::numbers::pi)) / static_cast<T>(12)
)
{
    this->series_name = "1/12*(3x^2-pi^2)";
    // Сходится при |x| ≤ π (ряд Фурье для функции f(x) = x²)
    // Расходится при |x| > π

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be ≤ π");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T one_twelfth_3x2_pi2_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(n + 1);
    return series_base<T, K>::minus_one_raised_to_power_n(n + 1) * cos(this->x * a) / (a * a); // (18.2) [Rows.pdf]
}
