/**
 * @file remainders.h
 * @brief This file contains the various variants of remainders for Levin type transformations
 * @authors Naumov A.
 * @edited by Yurov P.
*/
#pragma once
#include "series_acceleration.h"


//=====================================================================

//Хм, такое ощущение, что они его сделали
//
//А потом такие : а нахуй, похуй
//
//Везде, где Т(1) замени на scale
//
//Просто в комментах укажи, что - то вроде :
//TODO посмотреть нахуй он нужен
//
//Смотри, потом мы будем "твикать" алгоритмы
//
//Пытаться улучшить или вывести свой
//
//Так что рили впиши везде scale вместо T(1)
//
//А где T(1) нет, по фану добавь умножение на scale
//И везде впиши для @param scale : //TODO посмотреть нахуй он нужен


//=====================================================================






/**
* @brief Abstract class for remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
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
	virtual T operator()(const K n, const K j, const series_base<T, K>* series, const T scale = T(1)) const = 0;
};


/**
* @brief Class for u variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
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

	T operator()(const K n, const K j, const series_base<T, K>* series, const T scale = T(1)) const {
		const T result = T(1) / scale / series->operator()(n + j);

		if (!std::isfinite(result))
			throw std::overflow_error("division by zero");

		return result;
	}
};


/**
* @brief Class for t variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
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

	T operator()(const K n, const K j, const series_base<T, K>* series, const T scale = T(1)) const {
		const T result = T(1) / series->operator()(n + j);

		if (!std::isfinite(result)) 
			throw std::overflow_error("division by zero");

		return result;
	}
};



/**
* @brief Class for t-wave variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
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

	T operator()(const K n, const K j, const series_base<T, K>* series, const T scale = T(1)) const {
		const T result = T(1) / series->operator()(n + j + 1);

		if (!std::isfinite(result)) 
			throw std::overflow_error("division by zero");
		
		return result;
	}
};


/**
* @brief Class for v-wave variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
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

	T operator()(const K n, const K j, const series_base<T, K>* series, const T scale = T(1)) const {
		const K nj = n + j;
		const T a1 = series->operator()(nj);
		const T a2 = series->operator()(nj + 1);
		const T result = (a2 - a1) / (a1 * a2);

		if (!std::isfinite(result)) 
			throw std::overflow_error("division by zero");

		return result;
	}
};

/**
* @brief Class for v-wave variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class v_transform_2 : public transform_base<T, K> {
public:

	/**
   * @brief Operator() function for computing v-remainder
   *	wn = an*an+1/(an-an+1)
   * @param n The number of terms in the partial sum.
   * @param k The order of transformation.
   * @param series The series from where to grab terms for remainders
   * @param scale is not nessesary
   * @return The partial sum after the transformation.
   */

	T operator()(const K n, const K j, const series_base<T, K>* series, const T scale = T(1)) const {
		const K nj = n + j;
		const T a1 = series->operator()(nj);
		const T a2 = series->operator()(nj + 1);
		const T result = (a1 - a2) / (a1 * a2);

		if (!std::isfinite(result)) 
			throw std::overflow_error("division by zero");

		return result;
	}
};