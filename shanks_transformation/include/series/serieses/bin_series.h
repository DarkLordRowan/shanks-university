
#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

/**
* @brief Binomial series ( (1+x)^a maclaurin series)
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class bin_series : public series_base<T, K>
{
    using series_base<T, K>::binomial_coefficient;

public:
    bin_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Bolshakov M.P.
    * @param x The argument for function series, alpha The integer constant
    * @param alpha
    */
    bin_series(T x, T alpha);

    /**
    * @brief Computes the nth term of the Binomial series
    * @authors Bolshakov M.P.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
private:

    /**
    * @brief The power
    * @authors Bolshakov M.P.
    */
    const T alpha;
};

template <typename T, typename K>
bin_series<T, K>::bin_series(T x, T alpha) : series_base<T, K>(x, std::pow(1 + x, alpha)), alpha(alpha)
{
    if (std::abs(x) > 1)
        throw std::domain_error("series diverge");
}

template <typename T, typename K>
constexpr T bin_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(binomial_coefficient(alpha, n) * std::pow(this->x, n));
}
