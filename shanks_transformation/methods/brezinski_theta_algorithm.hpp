/**
 * @file theta_brezinski_algorithm.h
 * @brief This file contains the declaration of the Theta Brezinski Algorithm class.
 */

#pragma once

#include "series_acceleration.hpp" // Include the series header

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class theta_brezinski_algorithm : public series_acceleration<T, K, series_templ>
{
protected:
    /**
     * @brief Recursive function to compute theta.
     * Computes the value of theta according to the given parameters.
     * @param n The number of terms in the partial sum.
     * @param k The order of transformation.
     * @return The value of theta.
     */
    T theta(K n, const K order, T S_n, const K j) const;

public:
    /*
     * @brief Parameterized constructor to initialize the Theta Brezinski Algorithm.
     * @authors Yurov P.I. Bezzaborov A.A.
     * @param series The series class object to be accelerated
     */
    explicit theta_brezinski_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

    /**
     * @brief Fast implementation of Theta Brezinski algorithm.
     * Computes the partial sum after the transformation using the Theta Brezinski Algorithm.
     * For more information, see p. 277 10.2-4 in [https://arxiv.org/pdf/math/0306302.pdf]
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */
    T operator()(const K n, const K order) const override;
};

#include "brezinski_theta_algorithm.tpp"