#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function 4 * artan(x)
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class four_arctan_series final : public series_base<T, K>
{
public:
    four_arctan_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Bolshakov M.P.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    four_arctan_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the arctan multiplied by four
    * @authors Bolshakov M.P.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

    constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return abs(x) > static_cast<T>(1) || !isfinite(x); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(x.real()) > static_cast<float_precision>(1) || !isfinite(x); 
		
		return false;

	}
};

template <Accepted T, std::unsigned_integral K>
four_arctan_series<T, K>::four_arctan_series(T x) : series_base<T, K>(x, static_cast<T>(4) * atan(x))
{
    this->series_name = "4*arctan(x)";
    // Сходится при |x| ≤ 1 (условно при |x| = 1)

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T four_arctan_series<T, K>::operator()(K n) const
{
    const T temp = static_cast<T>(fma(2, n, 1));
    return static_cast<T>(4) * series_base<T, K>::minus_one_raised_to_power_n(n) * pow(this->x, temp) / temp; // (7.1) Rows.pdf
}
