/**
 * @file rho-wynn_algorithm.h
 * @brief This file contains the declaration of the Rho Wynn Algorithm class.
 * @authors Yurov P.I. Bezzaborov A.A.
 */

#pragma once

#include "series_acceleration.h" // Include the series header
#include <memory> // For std::unique_ptr
#include "wynn_numerators.h"

 /**
  * @brief Rho Wynn Algorithm class template.
  * @tparam T The type of the elements in the series
  * @tparam K The type of enumerating integer
  * @tparam series_templ is the type of series whose convergence we accelerate
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class rho_Wynn_algorithm : public series_acceleration<T, K, series_templ>
{
protected:
	std::unique_ptr<const numerator_base<T, K>> numerator_func;
	const T gamma;
	const T RHO;

	T calculate(const K n, K order) const { //const int order
		if (order & 1) { // is order odd
			++order;
			throw std::domain_error("order should be even number");
		}

		if (order == 0)
			return this->series->S_n(n);

		const T S_n = this->series->S_n(n);

		const K order1 = order - 1;

		const T res = (recursive_calculate_body(n, order1 - 1, S_n, 1) + numerator_func->operator()(n, order, this->series, gamma, RHO)) / 
			(recursive_calculate_body(n, order1, S_n, 1) - recursive_calculate_body(n, order1, S_n, 0));

		if (!std::isfinite(res))
			throw std::overflow_error("division by zero");

		return res;
	}

	T recursive_calculate_body(const K n, const K order, T S_n, const K j) const {
		/**
		* S_n - previous sum;
		* j - adjusts n: (n + j);
		*/
		S_n += (j == K(0)) ? T(0) : this->series->operator()(n + j);
		if (order == 0)
			return S_n;

		if (order == -1)
			return 0;

		const K order1 = order - 1;
		const K nj = n + j;

		const T res = (recursive_calculate_body(nj, order1 - 1, S_n, 1) + numerator_func->operator()(nj, order, this->series, gamma, RHO)) /
			(recursive_calculate_body(nj, order1, S_n, 1) - recursive_calculate_body(nj, order1, S_n, 0));

		if (!std::isfinite(res))
			throw std::overflow_error("division by zero");

		return res;
	}
public:
	/**
     * @brief Parameterized constructor to initialize the Rho Wynn Algorithm.
     * @param series The series class object to be accelerated
     */
	explicit rho_Wynn_algorithm(
		const series_templ& series, 
		const numerator_variant variant = numerator_variant::rho_variant, 
		const T gamma_ = T(1), 
		const T RHO_ = T(0)
	);

	//Default destructor is sufficient since unique_ptr handles deletion

	/**
     * @brief Rho Wynn algorithm.
     * Computes the partial sum after the transformation using the Rho Wynn Algorithm.
     * For more information, see
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */
	T operator()(const K n, const K order) const {
		return calculate(n, order);
	}

	/**
	 * @brief Compute transformed partial sum (extended version for arbitrary precision)
	 * @tparam BigK Type for term count (auto-deduced)
	 * @tparam BigOrder Type for order (auto-deduced)
	 * @param n Number of terms (passed by reference for large types)
	 * @param order Transformation order (must be even, passed by reference)
	 * @return The accelerated partial sum
	 * @throws std::domain_error for invalid arguments
	 * @throws std::overflow_error for division by zero
	 */

	template <typename BigK, typename BigOrder, typename = std::enable_if_t<!std::is_same_v<BigK, K> || !std::is_same_v<BigOrder, K>>> T operator()(const BigK& n, const BigOrder& order) const {
		static_assert(std::is_constructible_v<K, BigK>, "Term count type must be convertible to K");

		return calculate_impl(static_cast<K>(n), static_cast<int>(order));
	}
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
rho_Wynn_algorithm<T, K, series_templ>::rho_Wynn_algorithm(
	const series_templ& series, 
	const numerator_variant variant,
	const T gamma_, 
	const T RHO_
	) : 
	series_acceleration<T, K, series_templ>(series), 
	gamma(gamma_), 
	RHO(RHO_) 
{
	//TODO: проверить можно ли использовать стандартные значения, указанный в конструктуре, в других вариантах + унифицировать названия
	// есть ли какие то ограничения на кастомные значения?
	// classic (0), gamma (1), gamma-rho (2): 
	// 0 -> gamma = 1, RHO = 0
	// 1 -> gamma = 2, RHO = 0
	// 2 -> gamma = 2, RHO = 1

	switch(variant) {
		case numerator_variant::rho_variant :
			numerator_func.reset(new rho_transform<T, K>());
			break;
		case numerator_variant::generalized_variant :
			numerator_func.reset(new rho_transform<T, K>());
			break;
		case numerator_variant::gamma_rho_variant :
			numerator_func.reset(new rho_transform<T, K>());
			break;
		default:
			numerator_func.reset(new rho_transform<T, K>());
	}
}