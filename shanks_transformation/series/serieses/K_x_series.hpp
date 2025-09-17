#pragma once
#include "../series_base.hpp"

/**
* @brief Taylor series of Complete elliptic integral of first kind K
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class K_x_series final : public series_base<T, K>
{
public:
    K_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    K_x_series(T x);

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
			return abs(x) >= static_cast<T>(1) || !isfinite(x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(x) >= static_cast<float_precision>(1) || !isfinite(x);

		return false;
	}
};

template <Accepted T, std::unsigned_integral K>
K_x_series<T, K>::K_x_series(T x) : series_base<T, K>(x)
{
    this->series_name = "K(x)";
    // Сходится при |x| < 1 (полный эллиптический интеграл первого рода)
    // Расходится при |x| ≥ 1

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T K_x_series<T, K>::operator()(K n) const
{
    if (n == 0)
        return static_cast<T>(std::numbers::pi * 0.5);

    const T coeff = static_cast<T>(this->double_fact(2*n-1) / this->double_fact(2*n));

    return static_cast<T>(std::numbers::pi * 0.5) * coeff * coeff * pow(this->x, static_cast<T>(2*n)); // (71.2) [Rows.pdf]
}
