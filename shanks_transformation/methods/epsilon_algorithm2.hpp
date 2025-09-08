/**
 * @file epsilon_algorithm_two.h
 * @brief This file contains the declaration of the second implementation of Epsilon Algorithm class.
 */

#pragma once

#include "series_acceleration.hpp" // Include the series header
#include <vector> // Include the vector library

 /**
  * @brief Epsilon Algorithm MK-2 class template. "Scalar Epsilon Algorithm"
  //SOME RESULTS CONCERNING THE FUNDAMENTAL NATURE OF WYNN'S VECTOR EPSILON ALGORITHM - same algo + vector form
  //On a Device for Computing the e (S ) Transformation - nothing new, just matrix
  //euler algoritm A_Note_on_the_Generalised_Euler_Transformation-Wynn-1971 - has Euler, but for um = z^m * v_m
  * @authors Kreinin R.G.
  * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class epsilon_algorithm_two : public series_acceleration<T, K, series_templ>
{
public:
    /**
     * @brief Parameterized constructor to initialize the Epsilon Algorithm MK-2.
     * @param series The series class object to be accelerated
     */
    explicit epsilon_algorithm_two(const series_templ& series);

    /**
     * @brief Fast impimentation of Levin algorithm.
     * Computes the partial sum after the transformation using the Epsilon Algorithm.
     * For more information, see page 20-21 in [https://hal.science/hal-04207550/document]
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */
    T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
epsilon_algorithm_two<T, K, series_templ>::epsilon_algorithm_two(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T epsilon_algorithm_two<T, K, series_templ>::operator()(const K n, const K order) const
{

    using std::isfinite;

    if (n == static_cast<K>(0)) 
        throw std::domain_error("n = 0 in the input");

    if (order == static_cast<K>(0))
        return this->series->S_n(n);

    K k = static_cast<K>(2);
    k *= order;
    k += n;
    k -= (n & static_cast<K>(1));

    std::vector<std::vector<T>> e(
        4, 
        std::vector<T>(
            k + static_cast<K>(3), 
            static_cast<T>(0)
        )
    ); //4 vectors k+3 length containing four Epsilon Table rows 

    K j = k;
    do { //Counting first row of Epsilon Table
        e[3][j] = this->series->S_n(j);
    } while (--j > static_cast<K>(0));

    T a = static_cast<T>(0);

    K i1, i2;

    while (k > static_cast<K>(-1)) {
        for (K i = static_cast<K>(0); i < k; ++i) {

            i1 = i + static_cast<K>(1);
            i2 = i + static_cast<K>(2);

            e[0][i] = e[2][i1];
            e[0][i]+= static_cast<T>(1) / (e[3][i1] - e[3][i]); //Standart Epsilon Wynn algorithm

            if (!isfinite(e[0][i]) && i2 <= k) { //This algorithm is used if new elliment is corrupted.
                a = e[2][i2] * static_cast<T>(1) / (static_cast<T>(1) - e[2][i2] / e[2][i1]);

                a += e[2][i] * static_cast<T>(1) / (static_cast<T>(1) - e[2][i] / e[2][i1]);

                a -= e[0][i2] * static_cast<T>(1)/ (static_cast<T>(1) - e[0][i2] / e[2][i1]);

                e[0][i] = a / (static_cast<T>(1) + a / e[2][i1]);
            }

            if (!isfinite(e[0][i])) //If new element is still corrupted we just copy prev. element, so we will get result
                e[0][i] = e[2][i];         
        }

        std::swap(e[0], e[1]); //Swapping rows of Epsilon Table. First ine will be overwriteen next turn
        std::swap(e[1], e[2]);
        std::swap(e[2], e[3]);

        --k;
    }
    
    if (!isfinite(e[0][0]))
        throw std::overflow_error("division by zero");

    return e[0][0];  //Only odd rows have mathmatical scense. Always returning e[0][0]
}