/**
* @brief Numerical series representation of pi/3
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
#pragma once

#include "../series_base.h"


template<typename T, typename K>
class pi_3_series final : public series_base<T, K> {
public:
    ~pi_3_series() override = default;

    pi_3_series();

    /**
    * @brief Computes the nth term of the Numerical series of pi/3
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template<typename T, typename K>
pi_3_series<T, K>::pi_3_series() : series_base<T, K>(0, static_cast<T>(std::numbers::pi / 3)) {
}

template<typename T, typename K>
constexpr T pi_3_series<T, K>::operator()(K n) const {
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(1.0 / ((n + 1) * (2 * n + 1) * (4 * n + 1)));
}
