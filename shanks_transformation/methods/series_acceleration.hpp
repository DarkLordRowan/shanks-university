/**
 * @file series_acceleration.hpp
 * @brief This file contains the declaration of the base class series_acceleration
 *
 * For theory, see:
 * Levin, D., & Sidi, A. (1981). Two new classes of nonlinear transformations for accelerating
 * the convergence of infinite integrals and series. Applied Mathematics and Computation, 9(2), 175-215.
 * Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications. Cambridge University Press.
 */

#pragma once

#include "../custom_concepts.hpp"
#include "../series/series_base.hpp"

#include <iostream>
#include <memory>

 /**
  * @brief Base class for series acceleration transformations
  * @authors Bolshakov M.P.
  *
  * This class serves as a base for implementing various series acceleration methods
  * such as Shanks transformation and Wynn's epsilon algorithm. It provides the
  * fundamental interface for computing accelerated partial sums of series.
  *
  * The class is designed to work with any series type that provides:
  * - T operator()(K n) const: returns the n-th series term aₙ
  * - T S_n(K n) const: returns the n-th partial sum sₙ = a₀ + ... + aₙ
  *
  * @tparam T Floating-point type for series elements (must satisfy std::floating_point)
  *           Represents numerical precision (float, double, long double)
  * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
  *           Used for counting and indexing operations (typically size_t, unsigned int, etc.)
  * @tparam series_templ Type of series object to accelerate. Must provide the required interface.
  */
template<AcceptedLike T, UnsignedIntLike K>
class series_acceleration
{
public:

    /**
     * @brief Parameterized constructor to initialize the series acceleration object
     * @authors Bolshakov M.P.
     * @param series The series class object to be accelerated
     *        Must be a valid object implementing the required series interface
     */
    explicit series_acceleration(std::string name  = "unknown");

    /**
     * @brief Method for printing basic information about the acceleration object
     * @authors Bolshakov M.P.
     *
     * Outputs the type name of the derived transformation class to standard output.
     * This is primarily used for debugging and identification purposes.
     */
    constexpr void print_info() const;

    /**
     * @brief Virtual function operator for computing the accelerated partial sum
     * @authors Bolshakov M.P., Pashkov B.B.
     *
     * This pure virtual function must be implemented by derived classes to provide
     * specific acceleration algorithms. It computes the transformed partial sum
     * using n terms of the series with the specified transformation order.
     *
     * @param n The number of terms to use in the transformation
     *        Valid values: n > 0 (algorithm requires at least 1 term)
     *        Higher values use more terms but may provide better acceleration
     * @param order The order of transformation (specific meaning depends on derived class)
     *        Valid values: order >= 0 (typically a non-negative integer)
     * @return The accelerated partial sum after applying the transformation
     * @throws May throw domain_error or overflow_error in derived implementations
     */
    virtual T operator()(K n, K order, K offset = static_cast<K>(0)) const = 0;

    void reset(const SeriesResult<T>& series, unsigned short int precision = 0);

protected:

    //const std::vector<T>* Sn = nullptr;
    //const std::vector<T>* an = nullptr;
    std::vector<T> Sn;
    std::vector<T> an;

    std::string acceleration_name = "series acceleration base class";

    unsigned short precision = 0;

};


/**
 * @brief Constructor implementation for series_acceleration
 *
 * Initializes the base class with the provided series object. This constructor
 * is typically called by derived classes through their constructor initializer lists.
 *
 * @param series The series object to be accelerated
 */
template<AcceptedLike T, UnsignedIntLike K>
series_acceleration<T, K>::series_acceleration(std::string name) : acceleration_name(name){}


/**
 * @brief Implementation of print_info method
 *
 * Outputs the name of the actual derived class type using RTTI (run-time type information).
 * This helps identify which specific acceleration method is being used.
 */
template<AcceptedLike T, UnsignedIntLike K>
constexpr void series_acceleration<T, K>::print_info() const { std::cout << this->acceleration_name << '\n'; }

template<AcceptedLike T, UnsignedIntLike K>
void series_acceleration<T, K>::reset(const SeriesResult<T>& series, unsigned short int precision){

    Sn = series.Sn;
    an = series.an;

    if constexpr (std::is_same<T, float_precision>::value){

        if(precision == 0){
            this->precision = series.Sn.at(0).precision();
        } else {
            this->precision = precision;
        }

    } else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){

        if(precision == 0){
            this->precision =  std::max(series.Sn.at(0).real().precision(), series.Sn.at(0).imag().precision());
        } else {
            this->precision = precision;
        }

    } else {

        this->precision = 0;
    }
    
};