#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function artanh(x)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class artanh_x_series final : public series_base<T, K>
{
public:
    artanh_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    artanh_x_series(T x);

    /**
    * @brief Computes the nth term of the artanhx series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the artanhx series
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
artanh_x_series<T, K>::artanh_x_series(T x) : series_base<T, K>(x, atanh(x))
{
    this->series_name = "artanh(x)";
    // Сходится при |x| < 1 (ряд для гиперболического арктангенса)
    // Расходится при |x| ≥ 1

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T artanh_x_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(fma(static_cast <K>(2), n, static_cast <K>(1)));
    return pow(this->x, a) / a;  // (86.1) [Rows.pdf]
}
