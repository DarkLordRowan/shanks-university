
/**
* @brief Maclaurin series of arctan multiplied by four
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class four_arctan_series final : public series_base<T, K>
{
public:
    four_arctan_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Bolshakov M.P.
    * @param x The argument for function series
    */
    explicit four_arctan_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the arctan multiplied by four
    * @authors Bolshakov M.P.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;

private:
    /**
    * @brief Computes nth term of the series
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @return nth term of the series
    */
    // T acsess_row(K n);
};

template <typename T, typename K>
four_arctan_series<T, K>::four_arctan_series(T x) : series_base<T, K>(x, 4 * std::atan(x))
{
    if (std::abs(x) > 1)
        throw std::domain_error("the arctan series diverge at x = " + std::to_string(x));
}

template <typename T, typename K>
T four_arctan_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(4 * series_base<T, K>::minus_one_raised_to_power_n(n) * std::pow(this->x, 2 * n + 1) / (2 * n + 1));
}
