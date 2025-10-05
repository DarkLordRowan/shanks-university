/**
 * @file drummond_d_algorithm.hpp
 * @brief Contains implementation of Drummond's D-transformation for sequence acceleration
 * @authors Naumov A.
 *
 * For theory, see:
 * Drummond, J.E. (1976). A method for the summation of slowly convergent series.
 * Osada, N. (1993). Acceleration Methods for Slowly Convergent Sequences and Their Applications.
 * Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications, Section 9.5.
 */

#pragma once

#include "../series_acceleration.hpp"
#include "../remainders.hpp"
#include <memory>					  // Include for unique ptr

 /**
  * @brief Drummond's D-transformation class template for accelerating slowly convergent series.
  *
  * This transformation is particularly effective for alternating series and sequences
  * with specific remainder behavior patterns. It uses different remainder variants
  * to handle various types of slowly convergent sequences.
  *
  * References:
  * - Drummond, J.E. (1976). A method for the summation of slowly convergent series.
  * - Osada, N. (1993). Acceleration Methods for Slowly Convergent Sequences and Their Applications.
  * - Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications.
  *
  * @tparam T Floating-point type for series elements (must satisfy Accepted)
  *           Represents numerical precision (float, double, long double)
  *           Used for all mathematical computations and storage
  * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
  *           Used for counting terms, indexing operations, and order specification
  *           Valid values: K >= 0, typically size_t or unsigned int
  * @tparam series_templ Type of series object to accelerate. Must provide:
  *           - T operator()(K n) const: returns the n-th series term a_n
  *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
  *           - T minus_one_raised_to_power_n(K n) const: returns (-1)^n
  *           - T binomial_coefficient(T n, K k) const: returns binomial coefficient C(n, k)
  */
template<AcceptedLike T, UnsignedIntLike K>
class drummond_d_algorithm final : public series_acceleration<T, K>
{
protected:

    std::unique_ptr<const transform_base<T, K>> remainder;  /**< Remainder estimator object */
    bool useRecFormulas = false;							/**< Flag indicating whether to use recurrence formulas */
    remainder_type variant = remainder_type::u_variant;		/**< Type of remainder variant to use */

	/**
	 * @brief Calculates D-transformation directly using the explicit formula.
	 *
	 * For theory, see: Drummond (1976), Eq. (2.1) and Sidi (2003), Section 9.5-4
	 * D_n^{(k)} = [Σ_{j=0}^n (-1)^j C(n, j) w_{n,j} S_{n+j}] / [Σ_{j=0}^n (-1)^j C(n, j) w_{n,j}]
	 * where w_{n,j} is the remainder estimator function.
	 *
	 * @param n The starting index for partial sums (S_n)
	 *        Valid values: n >= 0, determines the starting point of transformation
	 * @param order The order of transformation (number of terms to use)
	 *        Valid values: order >= 0, higher orders use more terms but may provide better acceleration
	 * @return The accelerated partial sum after D-transformation
	 * @throws std::overflow_error if division by zero occurs
	 */
	inline T calc_result(
        K n, 
        K order, 
        K offset = static_cast<K>(0)
    ) const;

	/**
	 * @brief Calculates D-transformation using recurrence relations for improved efficiency.
	 *
	 * For theory, see: Sidi (2003), Section 9.5-5
	 * Implements the recursive computation scheme for Drummond's transformation
	 * using forward difference operations on the remainder-weighted partial sums.
	 *
	 * @param n The starting index for partial sums (S_n)
	 *        Valid values: n >= 0, determines the starting point of transformation
	 * @param order The order of transformation (number of terms to use)
	 *        Valid values: order >= 0, higher orders use more terms but may provide better acceleration
	 * @return The accelerated partial sum after D-transformation
	 * @throws std::overflow_error if division by zero occurs
	 */
	inline T calc_result_rec(
        K n, 
        K order, 
        K offset = static_cast<K>(0)
    ) const;

public:

	/**
	 * @brief Parameterized constructor to initialize Drummond's D-algorithm.
	 *
	 * @param series The series object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 * @param variant Type of remainder estimator to use
	 *        Determines the specific variant of Drummond's transformation:
	 *        - u_variant: Standard remainder estimator
	 *        - t_variant: Alternative remainder estimator
	 *        - v_variant: Alternative remainder estimator
	 *        - t_wave_variant: Modified remainder estimator
	 *        - v_wave_variant: Modified remainder estimator
	 * @param useRecFormulas Flag indicating whether to use recurrence formulas
	 *        true: Use recursive computation (better for large orders)
	 *        false: Use direct computation (simpler but potentially slower)
	 */
	explicit drummond_d_algorithm(
		remainder_type variant = remainder_type::u_variant,
		bool useRecFormulas = false
	);

	/**
	 * @brief Applies Drummond's D-transformation to accelerate series convergence.
	 *
	 * Computes the accelerated sum using Drummond's method with the specified
	 * remainder variant. The transformation is particularly effective for
	 * alternating series and sequences with specific convergence patterns.
	 *
	 * For theory, see: Drummond (1976), Main Theorem and Sidi (2003), Theorem 9.5.1
	 *
	 * @param n The starting index for partial sums (S_n)
	 *        Valid values: n >= 0, determines the starting point of transformation
	 *        Higher values use more stable terms but may converge slower
	 * @param order The order of transformation
	 *        Valid values: order >= 0, higher orders use more terms but may provide better acceleration
	 *        Typically order <= 10 for numerical stability
	 * @return The accelerated partial sum after Drummond transformation
	 * @throws std::overflow_error if division by zero or numerical instability occurs
	 */
    T operator()(K n, K order, K offset) const override;

};

template<AcceptedLike T, UnsignedIntLike K>
inline T drummond_d_algorithm<T,K>::calc_result(
    K n, 
    K order, 
    K offset
) const {

    using std::isfinite;

	T numerator = convertWithPrec<T>(0.0, series_acceleration<T,K>::precision);
	T denominator = convertWithPrec<T>(0.0, series_acceleration<T,K>::precision);
	T rest = convertWithPrec<T>(0.0, series_acceleration<T,K>::precision);

	// For theory, see: Drummond (1976), Eq. (2.1)
	// D_n^{(k)} = [Σ_{j=0}^n (-1)^j C(n, j) w_{n,j} S_{n+j}] / [Σ_{j=0}^n (-1)^j C(n, j) w_{n,j}]
	for (K j = static_cast<K>(0); j <= order; ++j) {

		// Compute weight term: (-1)^j * C(n, j) * w_{n,j}
		rest  = minus_one_raised_to_power_n<T,K>(j);
		rest *= static_cast<T>(binomial_coefficient(order, j));
		rest *= remainder->operator()(n,offset + j, series_acceleration<T, K>::an);

		numerator   += rest * series_acceleration<T, K>::Sn.at(n+j);
		denominator += rest;
	}

	numerator /= denominator;

	if (!isfinite(numerator))
		throw std::overflow_error("division by zero");
	return numerator;
}

template<AcceptedLike T, UnsignedIntLike K>
inline T drummond_d_algorithm<T,K>::calc_result_rec(
    K n, 
    K order, 
    K offset
) const {

    using std::isfinite;

	// For theory, see: Sidi (2003), Section 9.5-5
	// Recursive computation using forward differences
	std::vector<T>   Num(
		order + static_cast<K>(1), 
		convertWithPrec<T>(0.0, series_acceleration<T,K>::precision)
	);
	std::vector<T> Denom(
		order + static_cast<K>(1), 
		convertWithPrec<T>(0.0, series_acceleration<T,K>::precision)
	);

	// Initialize base values: N_j^{(0)} = w_{n,j} S_{n+j}, D_j^{(0)} = w_{n,j}
	for (K i = static_cast<K>(0); i < order+static_cast<K>(1); ++i) {
		Denom[i] = remainder->operator()(n, offset + i, series_acceleration<T, K>::an);
		  Num[i] = series_acceleration<T, K>::Sn.at(offset + i) * Denom[i];
	}

	// Apply forward difference recurrence:
	// N_j^{(i)} = N_{j+1}^{(i-1)} - N_j^{(i-1)}
	// D_j^{(i)} = D_{j+1}^{(i-1)} - D_j^{(i-1)}
	for (K i = static_cast<K>(1); i <= order; ++i)
		for (K j = static_cast<K>(0); j <= order - i; ++j) {
			Denom[j] = Denom[j+static_cast<K>(1)] - Denom[j];
			  Num[j] =   Num[j+static_cast<K>(1)] -   Num[j];
		}

	// Final result: D_n^{(order)} = N_0^{(order)} / D_0^{(order)}
	Num[0] /= Denom[0];

	if (!isfinite(Num[0]))
		throw std::overflow_error("division by zero");

	return Num[0];
}

template<AcceptedLike T, UnsignedIntLike K>
drummond_d_algorithm<T,K>::drummond_d_algorithm(
	const remainder_type variant,
	bool useRecFormulas
	) :
	series_acceleration<T, K>(),
	variant(variant),
	useRecFormulas(useRecFormulas)
{

	series_acceleration<T, K>::acceleration_name = (useRecFormulas ? "recurrent " : "");

	// Initialize the appropriate remainder estimator based on variant
    switch(variant){
        case remainder_type::u_variant :
		{
			series_acceleration<T, K>::acceleration_name += "drummond d algorithm with u-variant";
            remainder.reset(new u_transform<T, K>());
            break;
		}
        case remainder_type::t_variant :
		{
			series_acceleration<T, K>::acceleration_name += "drummond d algorithm with t-variant";
            remainder.reset(new t_transform<T, K>());
            break;
		}
        case remainder_type::v_variant :
		{
			series_acceleration<T, K>::acceleration_name += "drummond d algorithm with v-variant";
            remainder.reset(new v_transform<T, K>());
            break;
		}
        case remainder_type::t_wave_variant:
		{
			series_acceleration<T, K>::acceleration_name += "drummond d algorithm with t-wave-variant";
            remainder.reset(new t_wave_transform<T, K>());
            break;
		}
        case remainder_type::v_wave_variant:
		{
			series_acceleration<T, K>::acceleration_name += "drummond d algorithm with v-wave-variant";
            remainder.reset(new v_wave_transform<T, K>());
            break;
		}
        default:
		{
			series_acceleration<T, K>::acceleration_name += "drummond d algorithm with u-variant";
            remainder.reset(new u_transform<T, K>());
            break;
		}
    }
}

template<AcceptedLike T, UnsignedIntLike K>
T drummond_d_algorithm<T,K>::operator()(K n, K order, K offset) const {

    K required_size = order + offset + static_cast<K>(1);

    if (series_acceleration<T, K>::Sn.size() < required_size || series_acceleration<T, K>::an.size() < required_size){
        throw std::out_of_range("Sn or an is smaller than required to calculate D_{" + to_string(order) + "}^{" + to_string(n) + "}");
	}

    if (order == static_cast<K>(0)) {
        return series_acceleration<T, K>::Sn.at(n);
    }

    using std::isfinite;

    const T result = (useRecFormulas ? calc_result_rec(n,order, offset) : calc_result(n, order, offset));
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}
