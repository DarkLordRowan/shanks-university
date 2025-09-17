#pragma once
#include "../series_base.hpp"

/**
* @brief Taylor series of function arcsin(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class arcsin_x_series final : public series_base<T, K>
{
public:
    arcsin_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    arcsin_x_series(T x);

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
arcsin_x_series<T, K>::arcsin_x_series(T x) : series_base<T, K>(x, asin(x))
{
    this->series_name = "arcsin(x)";
    // Сходится при |x| ≤ 1 (биномиальный ряд для arcsin(x))
    // Расходится при |x| > 1

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T arcsin_x_series<T, K>::operator()(K n) const
{
    const T fact = static_cast<T>(this->fact(n));
    const T coeff = static_cast<T>(this->fact(2*n)) / fact / fact / static_cast<T>(pow(2, 2*n));
    return coeff * pow(this->x, static_cast<T>(fma(2,n,1))) / static_cast<T>(fma(2,n,1)); // (69.1) [Rows.pdf]
}
