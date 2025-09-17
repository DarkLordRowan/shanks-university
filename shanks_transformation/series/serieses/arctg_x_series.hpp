#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function arctg(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class arctg_x_series final : public series_base<T, K>
{
public:
    arctg_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    arctg_x_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

    constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return x > static_cast<T>(1.0) || !isfinite(x); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return x.real() > static_cast<float_precision>(1.0) || !isfinite(x); 
		
		return false;

	}
};

template <Accepted T, std::unsigned_integral K>
arctg_x_series<T, K>::arctg_x_series(T x) : series_base<T, K>(x, atan(x))
{
    this->series_name = "arctg(x)";
    // Сходится при |x| ≤ 1 (ряд для arctg(x) сходится абсолютно при |x| < 1, условно при |x| = 1)
    // Расходится при |x| > 1

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T arctg_x_series<T, K>::operator()(K n) const
{
    const T two_n_1 = static_cast<T>(fma(2, n, 1));
    return this->minus_one_raised_to_power_n(n) * pow(this->x, two_n_1) / two_n_1; // (70.1) [Rows.pdf], (1 - 2 * (n & 1)) = (-1)^{n}
}
