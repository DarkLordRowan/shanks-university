#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function arctan(x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class arctan_x2_series final : public series_base<T, K>
{
public:
    arctan_x2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    arctan_x2_series(T x);

    /**
    * @brief Computes the nth term of the arctan_x2 series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the arctan_x2 series
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
arctan_x2_series<T, K>::arctan_x2_series(T x) : series_base<T, K>(x, atan(x* x))
{
    this->series_name = "arctan(x²)";
    // Сходится при |x| ≤ 1 (ряд для arctan(z) сходится при |z| ≤ 1, где z = x²)
    // Расходится при |x| > 1

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T arctan_x2_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(fma(2,n,1));
    return series_base<T, K>::minus_one_raised_to_power_n(n) * pow(this->x, static_cast<T>(2) * a) / a; // (80.2) [Rows.pdf]
}
