#pragma once
#include "../series_base.hpp"

/**
* @brief Taylor series of function ((x^2 + 3) / (x^2 + 2*x)) - 1 at x = 1
* @authors Trudolyubov N.A.
* @tparam T - type of the elements in the series, K - type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class xsquareplus3_div_xsquareplus2multix_minus_1_series final : public series_base<T, K>
{
public:
    xsquareplus3_div_xsquareplus2multix_minus_1_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x - argument for function series
    */
    xsquareplus3_div_xsquareplus2multix_minus_1_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n - number of term
    * @return nth term of Taylor series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

    constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value)
			return abs(x) >= static_cast<T>(2) || !isfinite(x) || x == static_cast<T>(0); 

		if constexpr ( std::is_same<T, float_precision>::value || std::is_same<T, complex_precision<float_precision>>::value)
			return abs(x) >= static_cast<float_precision>(2) || !isfinite(x) || x == static_cast<T>(0);
		
		return false;

	}

};

template <Accepted T, std::unsigned_integral K>
xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>::xsquareplus3_div_xsquareplus2multix_minus_1_series(T x)
: series_base<T, K>(
    x,
    (x* x + static_cast<T>(3)) /( x * (x + static_cast<T>(2))) - static_cast<T>(1)
)
{
    this->series_name = "((x²+3)/x)*(x+2)-1";
    // Сходится при |x| < 2 (ряд Лорана, разложение в окрестности бесконечности)
    // Расходится при |x| >= 2

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be > 2 and x ≠ 0");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>::operator()(K n) const
{
    if (n == 0)
        return static_cast<T>(1.5) / this->x; //n = -1, но n отрицательным быть не может поэтому сдвигаю на 1

    return this->minus_one_raised_to_power_n(n) * static_cast<T>(7) * pow(this->x, static_cast<T>(n - 1)) / static_cast<T>(pow(2, n + 1)); // (68.1) [Rows.pdf]
}
