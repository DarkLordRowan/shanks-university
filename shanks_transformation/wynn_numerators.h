/**
 * @file wynn_numerators.h
 * @brief This file contains the various variants of numerator for Pho Wynn type transformations
 * @authors Yurov P.I. Bezzaborov A.A.
*/
#pragma once
#include "series_acceleration.h"

/**
* @brief Abstract class for numerator
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<typename T, typename K>
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
	virtual T operator()(const K& n, const int& order, const series_base<T, K>* series, T gamma = T(1), T rho = T(0)) const = 0;

};

/**
* @brief Class for rho variant of numerator
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<typename T, typename K>
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

	T operator()(const K& n, const int& order, const series_base<T, K>* series, T gamma = T(1), T rho = T(0)) const {
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

	T operator()(const K& n, const int& order, const series_base<T, K>* series, T gamma = T(1), T rho = T(0)) const {
		return (order - gamma - 1);
	}
};


/**
* @brief Class for gamma-rho variant of numerator
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<typename T, typename K>
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

	T operator()(const K& n, const int& order, const series_base<T, K>* series, T gamma = T(1), T rho = T(0)) const {
		return (-gamma + T(order/2)/rho + T(order%2));
	}
};