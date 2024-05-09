/**
 * @file theta_brezinski_algorithm.h
 * @brief This file contains the declaration of the Theta Brezinski Algorithm class.
 */

#pragma once

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library

template <typename T, typename K, typename series_templ>
class theta_brezinski_algorithm : public series_acceleration<T, K, series_templ>
{
public:
    /*
     * @brief Parameterized constructor to initialize the Theta Brezinski Algorithm.
     * @authors Yurov P.I. Bezzaborov A.A.
     * @param series The series class object to be accelerated
     */
    theta_brezinski_algorithm(const series_templ& series);

    /**
     * @brief Fast implementation of Theta Brezinski algorithm.
     * Computes the partial sum after the transformation using the Theta Brezinski Algorithm.
     * For more information, see p. 277 10.2-4 in [https://arxiv.org/pdf/math/0306302.pdf]
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */
    T operator()(const K n, const int order) const;

protected:
    /**
     * @brief Recursive function to compute theta.
     * Computes the value of theta according to the given parameters.
     * @param n The number of terms in the partial sum.
     * @param k The order of transformation.
     * @return The value of theta.
     */
    T theta(const int n, const int k) const;
};

template <typename T, typename K, typename series_templ>
theta_brezinski_algorithm<T, K, series_templ>::theta_brezinski_algorithm(const series_templ& series)
    : series_acceleration<T, K, series_templ>(series) {}

template <typename T, typename K, typename series_templ>
T theta_brezinski_algorithm<T, K, series_templ>::operator()(const K n, const int order) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    else if (n == 0 || order == 0)
        return this->series->S_n(n);
    return theta(n, order);
}


template <typename T, typename K, typename series_templ>
T theta_brezinski_algorithm<T, K, series_templ>::theta(const int n, const int k) const
{
    if (k == 1)
    {
        return 1/this->series->operator()(n+1); 
    }
    else if (k == 0)
    {
        return this->series->S_n(n);
    }
    else if (k % 2 == 1)
    {
        const T delta = theta(n, k - 1) - theta(n+1, k - 1); // Δυ_2k^(n)
        return theta(n+1, k - 2) + T(1) / delta; // υ_(2k+1)^(n)=υ_(2k-1)^(n+1) + 1/(Δυ_2k^(n)
    }
    else
    { // k % 2 == 0
        const T delta_n = theta(n + 1, k - 2) - theta(n+2 , k - 2); // Δυ_2k^(n+1) 
        const T delta_n1 = theta(n + 1, k - 1) - theta(n+2, k - 1); // Δυ_(2k+1)^(n+1)
        const T delta2 = theta(n, k - 1) - 2 * theta(n+1, k - 1) + theta(n+2, k - 1); // Δ^2 υ_(2k+1)^(n)
        return theta(n + 1, k - 2) + (delta_n * delta_n1) / delta2; // υ_(2k+2)^(n)=υ_2k^(n+1)+((Δυ_2k^(n+1))*(Δυ_(2k+1)^(n+1)))/(Δ^2 υ_(2k+1)^(n)
    }
}