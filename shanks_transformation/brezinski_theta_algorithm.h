/**
 * @file theta_brezinski_algorithm.h
 * @brief This file contains the declaration of the Theta Brezinski Algorithm class.
 */

#pragma once

#include "series_acceleration.hpp" // Include the series header

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class theta_brezinski_algorithm : public series_acceleration<T, K, series_templ>
{
public:
    /*
     * @brief Parameterized constructor to initialize the Theta Brezinski Algorithm.
     * @authors Yurov P.I. Bezzaborov A.A.
     * @param series The series class object to be accelerated
     */
    theta_brezinski_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

    /**
     * @brief Fast implementation of Theta Brezinski algorithm.
     * Computes the partial sum after the transformation using the Theta Brezinski Algorithm.
     * For more information, see p. 277 10.2-4 in [https://arxiv.org/pdf/math/0306302.pdf]
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */
    T operator()(const K n, const K order) const {
        if (order & 1) // is order odd?
            throw std::domain_error("order should be even number");

        if (n == 0 || order == 0)
            return this->series->S_n(n);

        return theta(n, order, this->series->S_n(n), 0);
    }

protected:
    /**
     * @brief Recursive function to compute theta.
     * Computes the value of theta according to the given parameters.
     * @param n The number of terms in the partial sum.
     * @param k The order of transformation.
     * @return The value of theta.
     */
    T theta(K n, const K order, T S_n, const K j) const {
        if (order == 1) {
            T res = 1 / this->series->operator()(n + j + 1);
            if (!std::isfinite(res))
                throw std::overflow_error("division by zero");

            return res;
        }

        for (K tmp = n + 1; tmp <= n + j; ++tmp)
            S_n += this->series->operator()(tmp);

        n += j;

        if (order == 0)
            return S_n;

        //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения
        const K order1 = order - 1;
        const K order2 = order - 2;

        const T theta_order1_0 = theta(n, order1, S_n, 0);
        const T theta_order1_1 = theta(n, order1, S_n, 1);
        const T theta_order1_2 = theta(n, order1, S_n, 2);
        const T theta_order2_1 = theta(n, order2, S_n, 1);

        if (order & 1) { // order is odd
            const T delta = T(1) / (theta_order1_0 - theta_order1_1); // 1/Δυ_2k^(n)

            if (!std::isfinite(delta))
                throw std::overflow_error("division by zero");

            return theta_order2_1 + delta; // υ_(2k+1)^(n)=υ_(2k-1)^(n+1) + 1/(Δυ_2k^(n)
        }
        // order is even

        const T delta2 = T(1) / static_cast<T>(fma(-2, theta_order1_1, theta_order1_0 + theta_order1_2)); // Δ^2 υ_(2k+1)^(n)

        if (!std::isfinite(delta2))
            throw std::overflow_error("division by zero");

        const T delta_n = theta_order2_1 - theta(n, order2, S_n, 2); // Δυ_2k^(n+1) 
        const T delta_n1 = theta_order1_1 - theta_order1_2; // Δυ_(2k+1)^(n+1)

        return static_cast<T>(fma(delta_n * delta_n1, delta2, theta_order2_1)); // υ_(2k+2)^(n)=υ_2k^(n+1)+((Δυ_2k^(n+1))*(Δυ_(2k+1)^(n+1)))/(Δ^2 υ_(2k+1)^(n)
    }
};