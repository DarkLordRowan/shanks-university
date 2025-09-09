/**
 * @file wynn_rho_algorithm.hpp
 * @brief This file contains the declaration of the Rho Wynn Algorithm class.
 * @authors Yurov P.I. Bezzaborov A.A.
 */

#pragma once


#include <memory> // For std::unique_ptr

#include "../series_acceleration.hpp"
#include "../wynn_numerators.hpp"

/**
  * @brief Rho Wynn Algorithm class template.
  * @tparam T The type of the elements in the series
  * @tparam K The type of enumerating integer
  * @tparam series_templ is the type of series whose convergence we accelerate
  */

//TODO: указать источники
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class wynn_rho_algorithm final : public series_acceleration<T, K, series_templ>
{
protected:

	std::unique_ptr<const numerator_base<T, K>> numerator;
	const T gamma;
	const T RHO;

	inline T calculate(K n, K order) const;

	T recursive_calculate_body(K n, K order, T S_n, K j) const;

public:
	/**
     * @brief Parameterized constructor to initialize the Rho Wynn Algorithm.
     * @param series The series class object to be accelerated
     */
	explicit wynn_rho_algorithm(
		const series_templ& series,
		numerator_type variant = numerator_type::rho_variant,
		T gamma_ = T(1),
		T RHO_ = T(0)
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
	T operator()(const K n, const K order) const override { return calculate(n, order); }

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
wynn_rho_algorithm<T, K, series_templ>::wynn_rho_algorithm(
	const series_templ& series,
	const numerator_type variant,
	const T gamma_,
	const T RHO_
	) :
	series_acceleration<T, K, series_templ>(series),
	gamma(gamma_),
	RHO(RHO_)
{
	//TODO: проверить можно ли использовать стандартные значения, указанные в конструктуре, в других вариантах + унифицировать названия
	// есть ли какие то ограничения на кастомные значения?
	// classic (0), gamma (1), gamma-rho (2):
	// 0 -> gamma = 1, RHO = 0
	// 1 -> gamma = 2, RHO = 0
	// 2 -> gamma = 2, RHO = 1

	switch(variant) {
		case numerator_type::rho_variant :
			numerator.reset(new rho_transform<T, K>());
			break;
		case numerator_type::generalized_variant :
			numerator.reset(new rho_transform<T, K>());
			break;
		case numerator_type::gamma_rho_variant :
			numerator.reset(new rho_transform<T, K>());
			break;
		default:
			numerator.reset(new rho_transform<T, K>());
	}
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T wynn_rho_algorithm<T, K, series_templ>::calculate(const K n, K order) const { //const int order

	using std::isfinite;

	if (order & static_cast<K>(1))// is order odd
		//++order; //why
		throw std::domain_error("order should be even number");

	if (order == static_cast<K>(0)) return this->series->S_n(n);

	const T S_n = this->series->S_n(n);
	const K order1 = order - static_cast<K>(1);
	const T res = 
		recursive_calculate_body(
			n,
			order1 - static_cast<K>(1),
			 S_n,
			 static_cast<K>(1)
		) +
		numerator->operator()(
			n,
			order,
			this->series,
			gamma,
			RHO
		) / (
		recursive_calculate_body(
			n,
			order1,
			S_n,
			static_cast<K>(1)
		) -
		recursive_calculate_body(
			n,
			order1,
			S_n,
			static_cast<K>(0)
		)
	);

	if (!isfinite(res))
		throw std::overflow_error("division by zero");
	return res;
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T wynn_rho_algorithm<T, K, series_templ>::recursive_calculate_body(const K n, const K order, T S_n, const K j) const {

	using std::isfinite;
	/**
	* S_n - previous sum;
	* j - adjusts n: (n + j);
	*/

	S_n += (j == static_cast<K>(0)) ? static_cast<T>(0) : this->series->operator()(n + j);

	if (order == static_cast<K>(0))
		return S_n;

	if (order == static_cast<K>(-1))
		return static_cast<T>(0);

	const K order1 = order - static_cast<K>(1);
	const K nj = n + j;
	const T res = 
		recursive_calculate_body(
			nj,
			order1 - static_cast<K>(1),
			S_n,
			static_cast<K>(1)
		) +
		numerator->operator()(
			nj,
			order,
			this->series,
			gamma,
			RHO
		) / (
		recursive_calculate_body(
			nj,
			order1,
			 S_n,
			 static_cast<K>(1)
		) -
		recursive_calculate_body(
			nj,
			order1,
			S_n,
			static_cast<K>(0)
		)
	);

	if (!isfinite(res))
		throw std::overflow_error("division by zero");

	return res;
}
