/**
* @brief x / sqrt(1 - x) serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template<typename T, typename K>
class x_div_1minx final : public series_base<T, K> {
public:
    x_div_1minx() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    */
    explicit x_div_1minx(T x);

    /**
    * @brief Computes the nth term of the x / sqrt(1 - x) series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @return nth term of the x / sqrt(1 - x) series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template<typename T, typename K>
x_div_1minx<T, K>::x_div_1minx(T x) : series_base<T, K>(x) {
}

template<typename T, typename K>
T x_div_1minx<T, K>::operator()(K n) const {
    if (n < 0)
        throw std::domain_error("The term index must be a non-negative integer");

    return static_cast<T>(this->fact(2 * n) * std::pow(this->x, n + 1) / (this->fact(n) * std::pow(4, this->x)));
}
