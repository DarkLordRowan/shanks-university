#pragma once
/**
 * @file epsilon_aitken_theta_algorithm.h
 * @brief This file contains the declaration of the Epsilon-Aitken-Theta Algorithm class.
 */

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library

 /**
  * @brief Epsilon-Aitken-Theta Algorithm class template.
  * @tparam T Element type of the series.
  * @tparam K Integer index type.
  * @tparam series_templ Series type to accelerate.
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class epsilon_aitken_theta_algorithm : public series_acceleration<T, K, series_templ>
{
public:
    /**
     * @brief Parameterized constructor to initialize the Theta Algorithm.
     * @authors Novak.M., Maximov.A.K.
     * @param series The series class object to be accelerated
     */
    epsilon_aitken_theta_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

    /**
     * @brief combizing of Epsilon Aitken Theta algorithms
     * Computes the partial sum after the transformation using the Epsilon, Aitken, Theta algo
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */

    T operator()(const K n, const K order) const {
        if (n < 4)
            return 0;

        std::vector<T> current(n);

        for (K i = 0; i < n; i++)
            current[i] = this->series->operator()(i);

        T delta, delta_next;
        T gamma, lambda;
        T ck;

        K i1, i2;

        for (K k = 1; k <= n; ++k) { // тут вместо n было iter, возможно iter - параметр, но если и так, то от него смысла нету, тк при iter > 1 ответ все время 0
            std::vector<T> step1(n, T(0));
            std::vector<T> step3(n, T(0));

            if (order == 1) {
                gamma = T(1) / 2 / k;
                lambda = T(1) - gamma;
            }
            else if (order == 2) {
                gamma = T(2) / 3 / k;
                lambda = T(1) - gamma;
            }
            else {
                gamma = T(1) / (k + 1);
                lambda = k * gamma;
            }

            /*
            T c_i1, c_i2, c_i3;
            T s1_i, s1_i1, s1_i2;
            */

            ck = T(1) + static_cast<T>(
                (order == 1) ? order / (2 * k - 1) :
                (order == 2) ? 1 / (3 * k - 2) :
                order / k
                );

            step1[0] = current[1] - ck * (current[1] - current[0]) / (current[2] - current[1]);

            for (K i = 0; i < K(step3.size() - 3); ++i) {
                i1 = i + 1;
                i2 = i + 2;

                step1[i1] = current[i2] - ck * (current[i2] - current[i1]) / (current[i2 + 1] - current[i2]);

                delta = step1[i1] - step1[i];

                delta_next = (i2 < K(step1.size())) ? step1[i2] - step1[i1] : T(0);

                step3[i] = gamma * step1[i] + lambda * (step1[i1] + (delta * delta_next) / (delta - delta_next));
            }
            current = step3;
        }

        const T res = current[n - 4];

        if (!std::isfinite(res))
            throw std::overflow_error("division by zero");

        return res;
    }
};