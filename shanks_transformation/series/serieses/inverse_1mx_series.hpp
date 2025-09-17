#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function 1 / (1 - x)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class inverse_1mx_series final : public series_base<T, K>
{
public:
    inverse_1mx_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    inverse_1mx_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of 1 / (1 - x)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

    constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return abs(x) >= static_cast<T>(1) || !isfinite(x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(x) >= static_cast<float_precision>(1) || !isfinite(x);

		return false;
	}

};

template <Accepted T, std::unsigned_integral K>
inverse_1mx_series<T, K>::inverse_1mx_series(T x) : series_base<T, K>(x, static_cast<T>(1) / (static_cast<T>(1) - x))
{
    this->series_name = "1/(1-x)";
    // Сходится при |x| < 1 (геометрический ряд)
    // Расходится при |x| ≥ 1

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T inverse_1mx_series<T, K>::operator()(K n) const
{
    return pow(this->x, static_cast<T>(n)); // (13.1) [Rows.pdf]
}
