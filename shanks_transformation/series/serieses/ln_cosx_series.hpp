#pragma once
#include "../series_base.hpp"
/**
* @brief Maclaurin series of function ln(cos(x))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class ln_cosx_series final : public series_base<T, K>
{
public:
    ln_cosx_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    ln_cosx_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

    constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value)
			return abs(x)  >= static_cast<T>(PI * 0.5) || !isfinite(x); 

        if constexpr (std::is_same<T, float_precision>::value)
            return abs(x)  >= static_cast<T>(0.5) * arbPI || !isfinite(x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(x) > static_cast<float_precision>(0.5) * arbPI || !isfinite(x); ; 
		
		return false;
	}
};

template <Accepted T, std::unsigned_integral K>
ln_cosx_series<T, K>::ln_cosx_series(T x) : series_base<T, K>(x, log(cos(x)))
{
    this->series_name = "ln(cos(x))";
    // Сходится при |x| < π/2 (ряд для логарифма косинуса)
    // Расходится при |x| ≥ π/2 (cos(x) ≤ 0, логарифм не определён)

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be < π/2");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T ln_cosx_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(fma(2, n, 2));
    return static_cast<T>(-1) * pow(sin(this->x), a) / a; // (50.2) [Rows.pdf]
}
