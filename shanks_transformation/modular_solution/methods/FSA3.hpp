/**
 * @file FSA_3.h
 * @brief This file contains the declaration of the Ford-Sidi Algorithm class.
 */

#pragma once

#include "series_acceleration.hpp" // Include the series header
#include <vector> // Include the vector library
#include <cmath>


template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class ford_sidi_algorithm_three : public series_acceleration<T, K, series_templ>{
public:

    /*    
     * @brief Parameterized constructor to initialize the Ford-Sidi V-3 Algorithm.
     * @authors Sharonov K.S.    
     * @param series The series class object to be accelerated
     */    
    explicit ford_sidi_algorithm_three(const series_templ& series);

    /*
     * @brief Fast implementation of Ford-Sidi.    
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.    
     * * @return The partial sum after the transformation.
     */    
    T operator()(const K n, const K k) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
ford_sidi_algorithm_three<T, K, series_templ>::ford_sidi_algorithm_three(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T ford_sidi_algorithm_three<T, K, series_templ>::operator()(const K n, const K order) const {  

    using std::isfinite;

    if (n == static_cast<K>(0))
        throw std::domain_error("zero integer in the input"); //TODO: унифицировать ошибку
    
    //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения
    const K n1 = n - static_cast<K>(1);
    const K m  = n + static_cast<K>(1);

    std::vector<T> G(m + static_cast<K>(1));
    std::vector<T> FSA(G.size());
    std::vector<T> FSI(G.size());
    std::vector<std::vector<T>> FSG(m + static_cast<K>(2), std::vector<T>(G.size()));

    G[0] = this->series->operator()(n1) * static_cast<T>(n);
    
    T Te = static_cast<T>(1) / static_cast<T>(n);
    
    for (K k = static_cast<K>(1); k <= m; ++k)
        G[k] = Te * G[k - 1];

    FSA[n1] = this->series->S_n(n1);
    FSI[n1] = static_cast<T>(1);

    if (G[0] != static_cast<T>(0)) {
        FSA[n1] /= G[0];
        FSI[n1] /= G[0];
        for (K i = static_cast<K>(1); i <= m; ++i)
            FSG[i][n1] = G[i] / G[0];
    } 
    else
        for (K i = static_cast<K>(1); i <= m; ++i)
            FSG[i][n1] = G[i];

    //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения
    K MM, MM1, k2;
    T D;

    // Основной цикл алгоритма Ford-Sidi
    for (K k = static_cast<K>(0); k <= n1; ++k) {
        MM = n1 - k;
        MM1 = MM + static_cast<K>(1);
        k2 = k + static_cast<K>(2);
        D = FSG[k2][MM1] - FSG[k2][MM];

        for (K i = k + static_cast<K>(3); i <= m; ++i)
            FSG[i][MM] = (FSG[i][MM1] - FSG[i][MM]) / D;

        FSA[MM] = (FSA[MM1] - FSA[MM]) / D;
        FSI[MM] = (FSI[MM1] - FSI[MM]) / D;
    }

    const T res = FSA[0] / FSI[0];

    if (!isfinite(res))
        throw std::overflow_error("division by zero");

    return res;
}