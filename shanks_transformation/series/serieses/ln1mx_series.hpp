#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function -ln(1 - x)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class ln1mx_series final : public series_base<T, K>
{
public:
    ln1mx_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    ln1mx_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of -ln(1 - x)
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
ln1mx_series<T, K>::ln1mx_series(T x) : series_base<T, K>(x, static_cast<T>(-1) * log(static_cast<T>(1) - x))
{
    this->series_name = "-ln(1-x)";
    // Сходится при |x| < 1 (абсолютно), условно сходится при x = -1
    // Расходится при x = 1

    if (domain_checker(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T ln1mx_series<T, K>::operator()(K n) const
{
    const T temp = static_cast<T>(n + 1);
    return pow(this->x, temp) / temp; // (8.2) [Rows.pdf]
}
