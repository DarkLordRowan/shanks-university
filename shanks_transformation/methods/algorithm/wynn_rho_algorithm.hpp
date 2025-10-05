/**
 * @file wynn_rho_algorithm.hpp
 * @brief This file contains the declaration of the Wynn's Rho Algorithm class.
 * @authors Yurov P.I. Bezzaborov A.A.
 *
 * For theory, see:
 * Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 * Wynn, P. (1966). Upon a second-order continued-fraction transformation for accelerating convergence.
 * Osada, N. (1993). Acceleration Methods for Slowly Convergent Sequences and Their Applications.
 */

#pragma once

#include <memory>

#include "../series_acceleration.hpp"
#include "../wynn_numerators.hpp"

 /**
  * @brief Wynn's Rho Algorithm class template for sequence acceleration.
  *
  * This class implements Wynn's rho algorithm, which is particularly effective
  * for accelerating logarithmically convergent sequences. The algorithm uses
  * continued fraction representations and reciprocal differences for convergence
  * acceleration.
  *
  * References:
  * - Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
  * - Wynn, P. (1966). Upon a second-order continued-fraction transformation for accelerating convergence.
  * - Osada, N. (1993). Acceleration Methods for Slowly Convergent Sequences and Their Applications.
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
  */
template <AcceptedLike T, UnsignedIntLike K>
class wynn_rho_algorithm final : public series_acceleration<T, K>
{
protected:

	std::unique_ptr<const numerator_base<T, K>> numerator;	/**< Numerator computation strategy */
	const T gamma;											/**< Gamma parameter for generalized rho transformation */
	const T RHO;											/**< Rho parameter for gamma-rho variant */

public:

	/**
	 * @brief Parameterized constructor to initialize the Rho Wynn Algorithm.
	 *
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 * @param variant Type of numerator computation strategy
	 *        Determines the specific variant of Wynn's rho algorithm:
	 *        - rho_variant: Standard Wynn rho algorithm
	 *        - generalized_variant: Generalized rho transformation
	 *        - gamma_rho_variant: Gamma-rho variant with additional parameters
	 * @param gamma_ Gamma parameter value (default: 1)
	 *        Valid values: T > 0, typically 1 or 2
	 *        Controls the transformation behavior in generalized variants
	 * @param RHO_ Rho parameter value (default: 0)
	 *        Valid values: T >= 0, typically 0 or 1
	 *        Additional parameter for gamma-rho variant
	 */


	// Стал:
	explicit wynn_rho_algorithm(
		numerator_type variant = numerator_type::rho_variant,
		const T& gamma_ = static_cast<T>(-1), // Передача по константной ссылке
		const T& RHO_   = static_cast<T>(1)  // Передача по константной ссылке
	);

	//Default destructor is sufficient since unique_ptr handles deletion

	/**
	 * @brief Wynn's rho algorithm transformation.
	 *
	 * Computes the accelerated partial sum using Wynn's rho algorithm.
	 * The algorithm is particularly effective for logarithmically convergent
	 * sequences and uses continued fraction representations for acceleration.
	 *
	 * For theory, see: Wynn (1966), Main Theorem
	 * ρ₂ₖ⁽ⁿ⁾ provides accelerated convergence for sequences satisfying
	 * S_n ~ S + Σ c_k n^{-k} as n → ∞
	 *
	 * @param n The starting index for partial sums
	 *        Valid values: n >= 0, determines the starting point of transformation
	 * @param order The order of transformation
	 *        Valid values: order >= 0 and must be even (order % 2 == 0)
	 *        Higher orders use more terms but may provide better acceleration
	 * @return The accelerated partial sum after rho transformation
	 * @throws std::domain_error if order is odd
	 * @throws std::overflow_error if division by zero occurs
	 */
	T operator()(
		const K n, 
        const K order,
		const SeriesResult<T>& data,
        const K offset = static_cast<K>(0)
	) const override;

};

template <AcceptedLike T, UnsignedIntLike K>
wynn_rho_algorithm<T, K>::wynn_rho_algorithm(
	const numerator_type variant,
	const T& gamma_,
	const T& RHO_
	) :
	series_acceleration<T, K>(),
	gamma(gamma_),
	RHO(RHO_)
{
	//TODO: проверить можно ли использовать стандартные значения, указанные в конструктуре, в других вариантах + унифицировать названия
	// есть ли какие то ограничения на кастомные значения?
	// classic (0), gamma (1), gamma-rho (2):
	// 0 -> gamma = 1, RHO = 0
	// 1 -> gamma = 2, RHO = 0
	// 2 -> gamma = 2, RHO = 1

	// Initialize the appropriate numerator computation strategy

	switch(variant) {
		case numerator_type::rho_variant :
		{
			numerator.reset(new rho_transform<T, K>());
			series_acceleration<T,K>::acceleration_name = "wynn rho with rho numerator";
			break;
		}
		case numerator_type::generalized_variant :
		{
			numerator.reset(new generilized_transform<T, K>());
			series_acceleration<T,K>::acceleration_name = "wynn rho with generalized numerator";
			break;
		}
		case numerator_type::gamma_rho_variant :
		{
			numerator.reset(new gamma_rho_transform<T, K>());
			series_acceleration<T,K>::acceleration_name = "wynn rho with rho numerator";
			break;
		}
		default:{

			series_acceleration<T,K>::acceleration_name = "wynn rho with rho numerator";
			numerator.reset(new rho_transform<T, K>());

		}
	}
}

template <AcceptedLike T, UnsignedIntLike K>
inline T wynn_rho_algorithm<T, K>::operator()(
	const K n, 
	const K order,
	const SeriesResult<T>& data,
	const K offset
) const { //const int order

	const K base_size = order + static_cast<K>(1) + offset;

    if (data.Sn.size() < base_size || data.an.size() < base_size){
        throw std::out_of_range("Sn or an is smaller than required to calculate theta_{" + to_string(order) + "}^{" + to_string(n) + "}");
	}
    // For theory, see: Brezinski (1977), Chapter 4, Eq. (4.10)
    // Base cases: return partial sum for n=0 or order=0
    if (order == static_cast<K>(0)){
    	return data.Sn.at(n);
	}

	size_t currentPrecision = std::max(
        series_acceleration<T, K>::define_precision(data.Sn[0]), 
        series_acceleration<T, K>::define_precision(data.an[0])
    );

	using std::isfinite;

    std::vector<T> rho_odd(
        base_size,
        convertWithPrec<T>(0.0, currentPrecision)
    ); // vector for theta_(2n + 1)

    std::vector<T> rho_even(
        base_size,
        convertWithPrec<T>(0.0,currentPrecision)
    ); //vector for theta_(2n), in the beginning it is theta_(-1) which is zero for all i

    // init theta_(0)
    for(K j = static_cast<K>(0); j < base_size; ++j) {
        rho_even[j] = data.Sn.at(offset + j);
	}


    K j1, j2;
    T delta; //temporary varaible

    for(K level = static_cast<K>(1); level <= order / static_cast<K>(2); ++level){

		// transform odd vector
        for(K j = static_cast<K>(0); j < base_size - level; ++j){

            j1 = j + static_cast<K>(1);

            delta = rho_even[j1] - rho_even[j];

			rho_odd[j] = rho_odd[j1] + numerator->operator()(
				n + j, 
				level * static_cast<K>(2) - static_cast<K>(1), 
				data.an, 
				gamma, 
				RHO
			) / delta;

        }


		// transform even vector
        for(K j = static_cast<K>(0); j < base_size - level; ++j){

            j1 = j + static_cast<K>(1);

            delta = rho_odd[j1] - rho_odd[j];
            
			rho_even[j] = rho_even[j1] + numerator->operator()(
				n + j, 
				level * static_cast<K>(2), 
				data.an, 
				gamma, 
				RHO
			) / delta;

        }

    }

    if(!isfinite(rho_even[0]))
        throw std::overflow_error("division by zero");

    return rho_even[0];
}