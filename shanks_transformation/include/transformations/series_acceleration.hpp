#ifndef SERIES_ACCELERATION_HPP
#define SERIES_ACCELERATION_HPP
#pragma once
/**
 * @file series_acceleration.hpp
 * @brief This file contains the declaration of the base class series_acceleration
 *
 * For theory, see:
 * Levin, D., & Sidi, A. (1981). Two new classes of nonlinear transformations for accelerating
 * the convergence of infinite integrals and series. Applied Mathematics and Computation, 9(2), 175-215.
 * Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications. Cambridge University Press.
 */


#include "../custom_concepts.hpp"
#include "../utils/utils_base.hpp"

#include <string>

 /**
  * @brief Base class for series acceleration transformations
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
  */
template<AcceptedLike T, UnsignedIntLike K>
class series_acceleration
{
public:

    /**
     * @brief Parameterized constructor to initialize the series acceleration object
    */
    explicit series_acceleration(std::string name  = "unknown") : acceleration_name(name) {};

    /**
     * @brief Virtual function operator for computing the accelerated partial sum
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
    virtual T operator()(const K n, const K order, const series_result<T>& data) const = 0;

    virtual std::string get_name() { return this->acceleration_name; }

protected:

    std::string acceleration_name = "series acceleration base class";

};

#endif