#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function arcsin(x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class arcsin_x2_series final : public series_base<T, K>
{
public:
    arcsin_x2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    arcsin_x2_series(T x);

    /**
    * @brief Computes the nth term of the arcsinx2 series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the arcsinx2 series
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
arcsin_x2_series<T, K>::arcsin_x2_series(T x) : series_base<T, K>(x, asin(x* x))
{
    this->series_name = "arcsin(x²)";
    // Сходится при |x| ≤ 1 (ряд для arcsin(z) сходится при |z| ≤ 1, где z = x²)
    // Расходится при |x| > 1

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T arcsin_x2_series<T, K>::operator()(K n) const
{
    const K a = static_cast<K>(fma(static_cast <K>(2), n, static_cast <K>(1)));
    return static_cast<T>(this->fact(a - static_cast <K>(1))) * pow(this->x, static_cast<T>(static_cast <K>(2) * a)) / static_cast<T>(pow(static_cast <K>(4), n) * pow(this->fact(n), static_cast <K>(2)) * a); // (84.1) [Rows.pdf]
}
