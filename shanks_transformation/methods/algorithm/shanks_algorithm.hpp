/**
 * @file shanks_algorithm.hpp
 * @brief This file contains the definition of the Shanks transformation class
 *        and specialization of this transformation for alternating series.
 */

 // For theory, see:
 // Shanks, D. (1955). Non-linear transformations of divergent and slowly convergent sequences.
 // Journal of Mathematics and Physics, 34(1-4), 1-42.
 // Senhadji, M.N. (2001). On condition numbers of the Shanks transformation.
 // Journal of Computational and Applied Mathematics, 135(1), 41-61.
 // Brezinski, C., He, Y., Hu, X.B., Redivo-Zaglia, M., & Sun, J.Q. (2010).
 // Multistep epsilon-algorithm, Shanks' transformation, and Lotka-Volterra system by Hirota's method.
 // Mathematics of Computation.

#pragma once

#include "../series_acceleration.hpp"
#include <vector>  // Include the vector library
#include <cmath> //Include for fma, isfinite

 /**
  * @brief Shanks transformation for non-alternating series class.
  * @tparam T The type of the elements in the series, K The type of enumerating integer,
  *         series_templ is the type of series whose convergence we accelerate
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class shanks_algorithm final : public series_acceleration<T, K, series_templ>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Shanks transformation for non-alternating series.
	 * @authors Bolshakov M.P.
	 * @param series The series class object
	 */
	explicit shanks_algorithm(const series_templ& series);

	/**
	 * @brief Shanks transformation for non-alternating function.
	 * @authors Bolshakov M.P., Pashkov B.B.
	 * @param n The number of terms in the partial sum.
	 * @param order The order of transformation.
	 * @return The partial sum after the transformation.
	 */
	T operator()(K n, K order) const;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
shanks_algorithm<T, K, series_templ>::shanks_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T shanks_algorithm<T, K, series_templ>::operator()(const K n, const K order) const{

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

		// For theory, see: Shanks (1955), Eq. (6) - Aitken's Δ² process
		// e₁(Sₙ) = Sₙ + (aₙaₙ₊₁)/(aₙ - aₙ₊₁) = (SₙSₙ₊₂ - Sₙ₊₁²)/(Sₙ₊₂ - 2Sₙ₊₁ + Sₙ)
		const T result = fma(
			a_n * a_n_plus_1,
			(a_n + a_n_plus_1) / (fma(a_n, a_n, tmp) - fma(a_n_plus_1, a_n_plus_1, tmp)), 
			this->series->S_n(n)
		);
		//n > order >= 1

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

		// For theory, see: Shanks (1955), Eq. (6) - First order transformation
		// e₁(Sᵢ) = Sᵢ + (aᵢaᵢ₊₁)/(aᵢ - aᵢ₊₁)
		T_n[i] = fma(
			a_n * a_n_plus_1, 
			(a_n + a_n_plus_1) / (fma(a_n, a_n, tmp) - fma(a_n_plus_1, a_n_plus_1, tmp)), 
			this->series->S_n(i)
		);
	}

	std::vector<T> T_n_plus_1(n + order, static_cast<T>(0));
	T a, b, c;

	for (K j = static_cast<K>(2); j <= order; ++j) {
		for (K i = n_minus_order + j; i <= n_plus_order - j; ++i) {
			a = T_n[i];
			b = T_n[i - static_cast<K>(1)];
			c = T_n[i + static_cast<K>(1)];

			// For theory, see: Shanks (1955), Eq. (12) - Higher order transformation
			// eₖ(Sₙ) = eₖ₋₁(Sₙ) + [eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ)] / [1 - (eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ))/(eₖ₋₁(Sₙ₊₂) - eₖ₋₁(Sₙ₊₁))]
			T_n_plus_1[i] = fma(
				fma(a, c + b - a, -b * c), 
				static_cast<T>(1) / (fma(static_cast<T>(2), a, -b - c)), 
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
* @tparam T The type of the elements in the series, K The type of enumerating integer,
*         series_templ is the type of series whose convergence we accelerate
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
	T operator()(K n, K order) const override;
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

		// For theory, see: Senhadji (2001), Section 3.2 - Alternating series case
		// For alternating series: e₁(Sₙ) = Sₙ + (aₙaₙ₊₁)/(aₙ - aₙ₊₁)
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

		// For theory, see: Senhadji (2001), Section 3.2 - Alternating series case
		// e₁(Sᵢ) = Sᵢ + (aᵢaᵢ₊₁)/(aᵢ - aᵢ₊₁)
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

			// For theory, see: Brezinski et al. (2010), Section 3 - Multistep transformation
			// Higher order transformation for alternating series
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