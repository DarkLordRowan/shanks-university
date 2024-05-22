/**
 * @file remainders.h
 * @brief This file contains the various variants of remainders for Levin type transformations
 * @authors Naumov A.
*/

#include "series_acceleration.h"

/**
* @brief Abstract class for remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<typename T, typename K>
class transform_base {
public:

	/**
   * @brief Virtual operator() function for computing remainder
   * @param n The number of terms in the partial sum.
   * @param k The order of transformation.
   * @param series The series from where to grab terms for remainders
   * @param scale The value to multiple (needed for u variant)
   * @return The partial sum after the transformation.
   */
	virtual T operator()(const int& n, const int& j, const series_base<T, K>* series, T scale = T(1)) const = 0;

};


/**
* @brief Class for u variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<typename T, typename K>
class u_transform : public transform_base<T, K> {
public:
	
	/**
   * @brief Operator() function for computing u-remainder
   *	wn = scale*an
   * @param n The number of terms in the partial sum.
   * @param k The order of transformation.
   * @param series The series from where to grab terms for remainders
   * @param scale The value to multiple the term, default is 1
   * @return The partial sum after the transformation.
   */

	T operator()(const int& n, const int& j, const series_base<T, K>* series, T scale = T(1)) const {
		return 1 / (scale * series->operator()(n + j));
	}
};


/**
* @brief Class for t variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<typename T, typename K>
class t_transform : public transform_base<T, K> {
public:

	/**
   * @brief Operator() function for computing t-remainder
   *	wn = an
   * @param n The number of terms in the partial sum.
   * @param k The order of transformation.
   * @param series The series from where to grab terms for remainders
   * @param scale is not nessesary
   * @return The partial sum after the transformation.
   */

	T operator()(const int& n, const int& j, const series_base<T, K>* series, T scale = T(1)) const {
		return 1 / series->operator()(n + j);
	}
};



/**
* @brief Class for t-wave variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<typename T, typename K>
class d_transform : public transform_base<T, K> {
public:

	/**
   * @brief Operator() function for computing t-wave-remainder
   *	wn = an+1
   * @param n The number of terms in the partial sum.
   * @param k The order of transformation.
   * @param series The series from where to grab terms for remainders
   * @param scale is not nessesary
   * @return The partial sum after the transformation.
   */

	T operator()(const int& n, const int& j, const series_base<T, K>* series, T scale = T(1)) const {
		return 1 / series->operator()(n + j + 1);
	}
};


/**
* @brief Class for v-wave variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<typename T, typename K>
class v_transform : public transform_base<T, K> {
public:

	/**
   * @brief Operator() function for computing v-remainder
   *	wn = an*an+1/(an+1-an)
   * @param n The number of terms in the partial sum.
   * @param k The order of transformation.
   * @param series The series from where to grab terms for remainders
   * @param scale is not nessesary
   * @return The partial sum after the transformation.
   */

	T operator()(const int& n, const int& j, const series_base<T, K>* series, T scale = T(1)) const {
		return (series->operator()(n + j + 1) - series->operator()(n + j)) / (series->operator()(n + j + 1) * series->operator()(n + j));
	}
};