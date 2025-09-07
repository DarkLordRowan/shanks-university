/**
 * @file shanks_transform.h
 * @brief This file contains the definition of the Shanks transformation class and specialization of this transformation for the alternating series.
 */

#pragma once

#include "series_acceleration.hpp" // Include the series header
#include <vector>  // Include the vector library

 /**
 * @brief Shanks transformation for non-alternating series class.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class shanks_transform : public series_acceleration<T, K, series_templ>
{
public:

	/**
   * @brief Parameterized constructor to initialize the Shanks transformation for non-alternating series.
   * @authors Bolshakov M.P.
   * @param series The series class object
   */
	explicit shanks_transform(const series_templ& series);

	/**
   * @brief Shanks transformation for non-alternating function.
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @return The partial sum after the transformation.
   */
	T operator()(const K n, const K order) const;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
shanks_transform<T, K, series_templ>::shanks_transform(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T shanks_transform<T, K, series_templ>::operator()(const K n, const K order) const{

	using std::isfinite;
	using std::fma;

	if (order == static_cast<K>(0)) [[unlikely]] /*it is convenient to assume that transformation of order 0 is no transformation at all*/
		return this->series->S_n(n);

	if (n < order || n == static_cast<K>(0)) [[unlikely]]
		return static_cast<T>(0); // TODO: диагностика

	if (order == static_cast<K>(1)) [[unlikely]] 
	{
		T a_n, a_n_plus_1, tmp;

		a_n = this->series->operator()(n);
		a_n_plus_1 = this->series->operator()(n + static_cast<K>(1));
		tmp = -a_n_plus_1 * a_n_plus_1;

		const T result = fma(
			a_n * a_n_plus_1,
			(a_n + a_n_plus_1) / (fma(a_n, a_n, tmp) - fma(a_n_plus_1, a_n_plus_1, tmp)), 
			this->series->S_n(n)
		);

		if (!isfinite(result))
			throw std::overflow_error("divison by zero");
		return result;
	}
	//n > order >= 1

	const K n_minus_order = n - order;
	const K n_plus_order = n + order;

	std::vector<T> T_n(n_plus_order, static_cast<T>(0));

	T a_n, a_n_plus_1, tmp;
	a_n = this->series->operator()(n_minus_order);
	a_n_plus_1 = this->series->operator()(n_minus_order + static_cast<K>(1));
	tmp = -a_n_plus_1 * a_n_plus_1;

	for (K i = n_minus_order + static_cast<K>(1); i <= n_plus_order - static_cast<K>(1); ++i) // if we got to this branch then we know that n >= order - see previous branches  int -> K
	{
		a_n = this->series->operator()(i);
		a_n_plus_1 = this->series->operator()(i + static_cast<K>(1));
		tmp = -a_n_plus_1 * a_n_plus_1;

		//formula[6]
		T_n[i] = fma(
			a_n * a_n_plus_1, 
			(a_n + a_n_plus_1) / (fma(a_n, a_n, tmp) - fma(a_n_plus_1, a_n_plus_1, tmp)), 
			this->series->S_n(i)
		);
	}

	std::vector<T> T_n_plus_1(n + order, static_cast<T>(0));
	T a, b, c;

	for (K j = static_cast<K>(2); j <= order; ++j) {
		for (K i = n_minus_order + j; i <= n_plus_order - j; ++i) { // int -> K
			a = T_n[i];
			b = T_n[i - static_cast<K>(1)];
			c = T_n[i + static_cast<K>(1)];
			T_n_plus_1[i] = fma(
				fma(a, c + b - a, -b * c), 
				static_cast<T>(1) / (fma(2, a, -b - c)), 
				a
			);
		}
		T_n = T_n_plus_1;
	}

	if (!isfinite(T_n[n]))
		throw std::overflow_error("division by zero");

	return T_n[n];
}

/**
* @brief Shanks transformation for alternating series class.
* @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
*/
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class shanks_transform_alternating : public series_acceleration<T, K, series_templ>
{
public:

	/**
   * @brief Parameterized constructor to initialize the Shanks transformation for alternating series.
   * @authors Bolshakov M.P.
   * @param series The series class object
   */
	explicit shanks_transform_alternating(const series_templ& series);

	/**
   * @brief Shanks transformation for alternating series function.
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @return The partial sum after the transformation.
   */
	T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
shanks_transform_alternating<T, K, series_templ>::shanks_transform_alternating(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T shanks_transform_alternating<T, K, series_templ>::operator()(const K n, const K order) const {

	using std::isfinite;
	using std::fma;

	if (order == static_cast<K>(0)) [[unlikely]] /*it is convenient to assume that transformation of order 0 is no transformation at all*/
		return this->series->S_n(n);

	if (n < order || n == static_cast<K>(0)) [[unlikely]]
		return static_cast<T>(0); // TODO: диагностика

	if (order == static_cast<K>(1)) [[unlikely]]
	{
		T a_n, a_n_plus_1, result;

		a_n = this->series->operator()(n);
		a_n_plus_1 = this->series->operator()(n + static_cast<K>(1));
		result = fma(
			a_n * a_n_plus_1, 
			static_cast<T>(1) / (a_n - a_n_plus_1), 
			this->series->S_n(n)
		);

		if (!isfinite(result))
			throw std::overflow_error("division by zero");

		return result;
	}
	//n > order >= 1

	const K n_minus_order = n - order;
	const K n_minus_order1 = n_minus_order + static_cast<K>(1);
	const K n_plus_order = n + order;

	std::vector<T> T_n(n_plus_order, static_cast<T>(0));

	T a_n, a_n_plus_1;

	a_n = this->series->operator()(n_minus_order);
	a_n_plus_1 = this->series->operator()(n_minus_order1);

	for (K i = n_minus_order1; i <= n_plus_order - static_cast<K>(1); ++i) // if we got to this branch then we know that n >= order - see previous branches int->K
	{
		a_n = this->series->operator()(i);
		a_n_plus_1 = this->series->operator()(i + static_cast<K>(1));

		// formula [6]
		T_n[i] = fma(
			a_n * a_n_plus_1, 
			static_cast<T>(1) / (a_n - a_n_plus_1), 
			this->series->S_n(n)
		);
	}

	std::vector<T> T_n_plus_1(n_plus_order, static_cast<T>(0));
	T a, b, c;

	for (K j = static_cast<K>(2); j <= order; ++j) {
		for (K i = n_minus_order + j; i <= n_plus_order - j; ++i) { // int -> K

			a = T_n[i];
			b = T_n[i - static_cast<K>(1)];
			c = T_n[i + static_cast<K>(1)];

			T_n_plus_1[i] = fma(
				fma(a, c + b - a, -b * c), 
				static_cast<T>(1) / (static_cast<T>(2) * a - b - c), 
				a
			);
		}
		T_n = T_n_plus_1;
	}
	
	if (!isfinite(T_n[n]))
		throw std::overflow_error("division by zero");

	return T_n[n];
}