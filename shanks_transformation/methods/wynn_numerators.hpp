/**
 * @file wynn_numerators.h
 * @brief This file contains the various variants of numerator for Pho Wynn type transformations
 * @authors Yurov P.I. Bezzaborov A.A.
*/
#pragma once

#include "../series.h"

/**
  * @brief Enum for remainders types to use in Levin-type transformations
*/
enum numerator_type{
           rho_variant,
   generalized_variant,
     gamma_rho_variant,
};

/**
 * @brief Abstract class for numerator
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 */

template<std::floating_point T, std::unsigned_integral K>
class numerator_base {
public:

	/**
     * @brief Virtual operator() function for computing numerator
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @param series The series from where to grab terms for numerator
     * @param gamma const for transformation	(	rho(gamma)		)
     * @param rho const for transformation	(	rho(gamma,rho)	)
     * @return The special numerator for transformation
     */
	virtual T operator()(K n, K order, const series_base<T, K>* series, T gamma = T(1), T rho = T(0)) const = 0;

};

/**
* @brief Class for rho variant of numerator
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class rho_transform : public numerator_base<T, K> {
public:

	/**
   * @brief Operator() function for computing rho numerator
   *	x_n+order - x_n
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @param series The series from where to grab terms for numerator
   * @param gamma const for transformation	(	rho(gamma)		)
   * @param rho const for transformation	(	rho(gamma,rho)	)
   * @return The special numerator for transformation
   */

	T operator()(const K n, const K order, const series_base<T, K>* series, const T gamma = static_cast<T>(1), const T rho = static_cast<T>(0)) const {
		return (series->operator()(n + order) - series->operator()(n));
	}
};


/**
* @brief Class for generilized variant of numerator
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<typename T, typename K>
class generilized_transform : public numerator_base<T, K> {
public:

	/**
   * @brief Operator() function for computing generilized numerator
   *	order-gamma-1
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @param series The series from where to grab terms for numerator
   * @param gamma const for transformation	(	rho(gamma)		)
   * @param rho const for transformation	(	rho(gamma,rho)	)
   * @return The special numerator for transformation
   */

	T operator()(const K n, const K order, const series_base<T, K>* series, const T gamma = static_cast<T>(1), const T rho = static_cast<T>(0)) const {
		return static_cast<T>(order) - gamma - static_cast<T>(1);
	}
};


/**
* @brief Class for gamma-rho variant of numerator
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class gamma_rho_transform : public numerator_base<T, K> {
public:

	/**
   * @brief Operator() function for computing gamma-rho numerator
   *	C_2j	 = -gamma + j/rho
   *	C_2j+1	 = -gamma + j/rho + 1
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @param series The series from where to grab terms for numerator
   * @param gamma const for transformation	(	rho(gamma)		)
   * @param rho const for transformation	(	rho(gamma,rho)	)
   * @return The special numerator for transformation
   */

	T operator()(const K n, const K order, const series_base<T, K>* series, const T gamma = static_cast<T>(1), const T rho = static_cast<T>(0)) const {

		// insight: order % 2 is the same order & 1
		// if order is even:
		// order % 2 = 0
		// order & 1 = 0
		// else order is odd:
		// order % 2 = 1
		// order & 1 = 1

		return (-gamma + static_cast<T>(order / static_cast<K>(2)) / rho + static_cast<T>(order & static_cast<K>(1)));
	}
};