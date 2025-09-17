#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series for function ln(1 + x^3)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class ln_1_plus_x3_series final : public series_base<T, K>
{
public:
    ln_1_plus_x3_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    ln_1_plus_x3_series(T x);

    /**
    * @brief Computes the nth term of the ln(1 + x^3) series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the ln(1 + x^3) series
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
ln_1_plus_x3_series<T, K>::ln_1_plus_x3_series(T x) : series_base<T, K>(x, log(static_cast<T>(1) + pow(x, static_cast<T>(3))))
{
    this->series_name = "ln(1+x³)";
    // Сходится при |x| ≤ 1 (ряд для ln(1+z) сходится при |z| ≤ 1, где z = x³)
    // Расходится при |x| > 1

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T ln_1_plus_x3_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(n + 1);
    return this->minus_one_raised_to_power_n(n + 2) * pow(this->x, static_cast<T>(3) * a) / a; // (99.3) [Rows.pdf]
}
