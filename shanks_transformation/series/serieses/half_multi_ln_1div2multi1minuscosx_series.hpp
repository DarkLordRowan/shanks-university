#pragma once
#include "../series_base.hpp"

/**
* @brief Trigonometric series of function 0.5 * ln(1/(2*(1 - cos(x))))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class half_multi_ln_1div2multi1minuscosx_series final : public series_base<T, K>
{
public:
    half_multi_ln_1div2multi1minuscosx_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    half_multi_ln_1div2multi1minuscosx_series(T x);

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
			return x <= static_cast<T>(0) || x >= static_cast<T>(2.0 * PI) || !isfinite(x) || cos(x) == static_cast<T>(1); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return x.real() <= static_cast<float_precision>(0) || x.real() >= static_cast<float_precision>(2.0) * arbPI || !isfinite(x) || cos(x) == static_cast<T>(1); 
		
		return false;

	}
};

template <Accepted T, std::unsigned_integral K>
half_multi_ln_1div2multi1minuscosx_series<T, K>::half_multi_ln_1div2multi1minuscosx_series(T x) : 
series_base<T, K>(
    x,
    static_cast<T>(0.5) * log(static_cast<T>(0.5) / (static_cast<T>(1) -  cos(x)))
    )
{
    this->series_name = "0.5*ln(1/(2*(1-cos(x))))";
    // Сходится при 0 < x < 2π (ряд для логарифма тригонометрической функции)
    // Расходится при x = 0, x = 2π и других точках, где cos(x) = 1

    if (domain_checker(x)) {
        this->throw_domain_error("x must be in (0, 2π) and cos(x) ≠ 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T half_multi_ln_1div2multi1minuscosx_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(n + 1);
    return cos(this->x * a) / a; // (47.5) [Rows.pdf]
}
