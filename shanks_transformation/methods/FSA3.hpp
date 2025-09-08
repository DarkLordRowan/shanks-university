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

#include "FSA3.tpp"