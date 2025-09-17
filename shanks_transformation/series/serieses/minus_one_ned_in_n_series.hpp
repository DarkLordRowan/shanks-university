#pragma once
#include "../series_base.hpp"

/**
* @brief Numerical series (-1)^n * x /n^n = -0,78343051 * x
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class minus_one_ned_in_n_series final : public series_base<T, K>
{
public:
    minus_one_ned_in_n_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    minus_one_ned_in_n_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

    constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return abs(x) > static_cast<T>(1) || !isfinite(x); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(x) > static_cast<float_precision>(1) || !isfinite(x); 
		
		return false;
	}
};

template <Accepted T, std::unsigned_integral K>
minus_one_ned_in_n_series<T, K>::minus_one_ned_in_n_series(T x) : series_base<T, K>(x, static_cast<T>(-0.78343051)* x) // -0.78343051 value only for initialization, we can calculate with more accuracy with operator()(K n)
{
    this->series_name = "∑(-1)ⁿ/n * x";
    // Сходится при |x| ≤ 1, x ≠ -1 (условно при x = 1, расходится при x = -1)
    // При |x| < 1 сходится абсолютно

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T minus_one_ned_in_n_series<T, K>::operator()(K n) const
{
    return (n ? this->x * series_base<T, K>::minus_one_raised_to_power_n(n) / static_cast<T>(pow(n, n)) : static_cast<T>(0)); // (37.2) [Rows.pdf]
}
