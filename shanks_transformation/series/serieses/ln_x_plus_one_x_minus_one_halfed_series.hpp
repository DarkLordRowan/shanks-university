#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function ln((x+1)/(1-x))
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class ln_x_plus_one_x_minus_one_halfed_series final : public series_base<T, K>
{
public:
    ln_x_plus_one_x_minus_one_halfed_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    ln_x_plus_one_x_minus_one_halfed_series(T x);

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
			return abs(x) >= static_cast<T>(1) || !isfinite(x); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(x) >= static_cast<float_precision>(1) || !isfinite(x); 
		
		return false;
	}
};

template <Accepted T, std::unsigned_integral K>
ln_x_plus_one_x_minus_one_halfed_series<T, K>::ln_x_plus_one_x_minus_one_halfed_series(T x) : 
series_base<T, K>(
    x, 
    log((static_cast<T>(1) + x) / (static_cast<T>(1) - x))
)
{
    this->series_name = "0.5*ln((1+x)/(1-x))";
    // Сходится при |x| < 1 (абсолютно)
    // Расходится при |x| ≥ 1

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T ln_x_plus_one_x_minus_one_halfed_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(fma(2, n, 1));
    return pow(this->x, a) / a; // (39.3) [Rows.pdf]
}
