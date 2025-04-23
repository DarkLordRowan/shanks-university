/**
 * @file FSA_3.h
 * @brief This file contains the declaration of the Ford-Sidi Algorithm class.
 */

#pragma once

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library
#include "series +.h" 
#include <cmath>


template <typename T, typename K, typename series_templ>
class ford_sidi_algorithm_three : public series_acceleration<T, K, series_templ>{
public:

    /** @brief Parameterized constructor to initialize the Ford-Sidi V-3 Algorithm.
      * @authors Sharonov K.S.    
      * @param series The series class object to be accelerated
    */    
    ford_sidi_algorithm_three(const series_templ& series, K n);

    /** 
    * @brief Fast implementation of Ford-Sidi.    
    * @param n The number of terms in the partial sum.
    * @param order The order of transformation.    
    * @return The partial sum after the transformation.
    */    
    T operator()(const K n, const int k) const;

protected:
    /** 
     * @brief Nmax - Maximum number of terms for acceleration.
     */
    K Nmax = 0;
    /** 
     * @brief FSG - Two-dimensional array storing intermediate transformation coefficients.
     */
    mutable std::vector<std::vector<T>> FSG; 
    /** 
     * @brief FSA - Accelerated numerator values vector.
     */
    mutable std::vector<T> FSA;
    /** 
     * @brief FSI - Accelerated denominator values vector.
     */
    mutable std::vector<T> FSI;
    /** 
     * @brief G - Weight coefficients vector.
     */
    mutable std::vector<T> G;
};


template <typename T, typename K, typename series_templ>
ford_sidi_algorithm_three<T, K, series_templ>::ford_sidi_algorithm_three(const series_templ& series, K n) :series_acceleration<T, K, series_templ>(series), FSG(n + 2, std::vector<T>(n + 1, 0.0)), FSA(n + 1, 0.0), FSI(n + 1, 0.0), G(n + 2, 0.0), Nmax(n) {}

template <typename T, typename K, typename series_templ>
T ford_sidi_algorithm_three<T, K, series_templ>::operator()(const K n, const int order) const{

    G[1] = this->series->operator()(n-1);
    
    T Te = 1.0 / (n);
    
    for (K k = 1; k <= this->Nmax + 1; ++k) {
        if (k == 1) {
            G[k] = G[k] * pow(n, k);
        } else {
            G[k] = Te * G[k - 1];
        }
    }

    if (abs(G[1]) >= 1e-77) {
        FSA[n - 1] = this -> series -> S_n(n-1) / G[1];
        FSI[n - 1] = 1.0 / G[1];
        for (int i = 2; i <= this->Nmax + 1; ++i) {
            FSG[i][n - 1] = G[i] / G[1];
        }
    } else {
        FSA[n - 1] = this -> series -> S_n(n-1);
        FSI[n - 1] = 1.0;
        for (int i = 2; i <= Nmax + 1; ++i) {
            FSG[i][n - 1] = G[i];
        }
    }

    for (K k = 0; k <= n - 1; ++k) {
        K MM = n - (k + 1);
        T D = FSG[k + 2][MM + 1] - FSG[k + 2][MM];
    
        for (K i = k + 3; i <= this->Nmax - 3; ++i) {
            FSG[i][MM] = (FSG[i][MM + 1] - FSG[i][MM]) / D;
        }
        FSA[MM] = (FSA[MM + 1] - FSA[MM]) / D;
        FSI[MM] = (FSI[MM + 1] - FSI[MM]) / D;                                                                                         

    }
    return FSA[0] / FSI[0];
}


