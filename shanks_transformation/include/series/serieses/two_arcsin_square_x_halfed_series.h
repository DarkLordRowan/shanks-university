/**
* @brief 2 arcsin(x/2)^2  p 567
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template<typename T, typename K>
class two_arcsin_square_x_halfed_series final : public series_base<T, K> {
public:
    two_arcsin_square_x_halfed_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    */
    explicit two_arcsin_square_x_halfed_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template<typename T, typename K>
two_arcsin_square_x_halfed_series<T, K>::two_arcsin_square_x_halfed_series(T x) : series_base<T, K>(
    x, static_cast<T>(2 * std::pow(std::asin(x / 2), 2))) {
}

template<typename T, typename K>
T two_arcsin_square_x_halfed_series<T, K>::operator()(K n) const {
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(pow(this->x, 2 * n + 2) * this->fact(n) * this->fact(n) / this->fact(2 * n + 2));
}
