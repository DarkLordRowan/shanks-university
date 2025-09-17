#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series for function cos(sqrt(x))
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class cos_sqrt_x_series final : public series_base<T, K>
{
public:
    cos_sqrt_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    cos_sqrt_x_series(T x);

    /**
    * @brief Computes the nth term of the cos(sqrt(x)) series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the cos(sqrt(x)) series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

    constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return x < static_cast<T>(0) || !isfinite(x); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return x.real() < static_cast<float_precision>(0) || !isfinite(x); 
		
		return false;

	}
};

template <Accepted T, std::unsigned_integral K>
cos_sqrt_x_series<T, K>::cos_sqrt_x_series(T x) : series_base<T, K>(x, cos(sqrt(x)))
{
    this->series_name = "cos(√x)";
    // Сходится при x ≥ 0 (ряд для cos(z) сходится при ∀z ∈ ℝ, где z = √x)
    // Расходится при x < 0 (комплексная область)

    if (domain_checker(x)) {
        this->throw_domain_error("x must be ≥ 0");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T cos_sqrt_x_series<T, K>::operator()(K n) const
{
    return series_base<T, K>::minus_one_raised_to_power_n(n) * pow(this->x, static_cast<T>(n)) / static_cast<T>(this->fact(2 * n)); // (98.2) [Rows.pdf]
}
