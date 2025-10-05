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
#include <vector>	// Include the vector library
#include <cmath>	//Include for fma, isfinite

/**
 * @brief Shanks transformation for non-alternating series class template.
 *
 * @tparam T Floating-point type for series elements (must satisfy Accepted)
 *           Represents numerical precision (float, double, long double)
 *           Used for all arithmetic operations and intermediate calculations
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting terms, indexing operations, and transformation order
 * @tparam series_templ Type of series object to accelerate. Must provide:
 *           - T operator()(K n) const: returns the n-th series term a_n
 *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
 */
template <AcceptedLike T, UnsignedIntLike K>
class shanks_algorithm final : public series_acceleration<T, K>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Shanks transformation for non-alternating series.
	 * @authors Bolshakov M.P.
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 *        Series terms should be non-alternating for optimal performance
	 */
	explicit shanks_algorithm() : series_acceleration<T, K>("shanks original") {};

	/**
	 * @brief Shanks transformation for non-alternating series function.
	 * @authors Bolshakov M.P., Pashkov B.B.
	 * @param n The number of terms in the partial sum to use for transformation
	 *        Valid values: n >= order > 0, n > 0
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (number of recursive applications)
	 *        Valid values: order >= 0 (order=0 returns untransformed partial sum)
	 *        Higher orders provide more aggressive acceleration but may be less stable
	 * @return The accelerated partial sum after Shanks transformation
	 * @throws std::overflow_error if division by zero or numerical instability occurs
	 */
	T operator()(
		const K n, 
        const K order,
		const SeriesResult<T>& data,
        const K offset = static_cast<K>(0)
	) const;
};

template <AcceptedLike T, UnsignedIntLike K>
T shanks_algorithm<T, K>::operator()(
	const K n, 
    const K order,
	const SeriesResult<T>& data,
    const K offset
) const{

    K required_size = order + offset + static_cast<K>(1);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("Sn or an is smaller than required to calculate shanks_{" + to_string(order) + "}^{" + to_string(n) + "}");
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;
	using std::fma;

	size_t currentPrecision = std::max(
        series_acceleration<T, K>::define_precision(data.Sn[0]), 
        series_acceleration<T, K>::define_precision(data.an[0])
    );

	if (order == static_cast<K>(1)) [[unlikely]]
	{
		T a_n = convertWithPrec<T>(0.0, currentPrecision);
		T a_n_plus_1 =convertWithPrec<T>(0.0, currentPrecision);
		T tmp = convertWithPrec<T>(0.0, currentPrecision);

		a_n += data.an.at(n);
		a_n_plus_1 += data.an.at(n + static_cast<K>(1));
		tmp -= a_n_plus_1 * a_n_plus_1;

		// For theory, see: Shanks (1955), Eq. (6) - Aitken's Δ² process
		// e₁(Sₙ) = (SₙSₙ₊₂ - Sₙ₊₁²)/(Sₙ₊₂ - 2Sₙ₊₁ + Sₙ) = Sₙ + (aₙaₙ₊₁)/(aₙ - aₙ₊₁)
		const T result = fma(
			a_n * a_n_plus_1,
			(a_n + a_n_plus_1) / (fma(a_n, a_n, tmp) - fma(a_n_plus_1, a_n_plus_1, tmp)),
			data.Sn.at(n)
		);
		//n > order >= 1

		if (!isfinite(result))
			throw std::overflow_error("divison by zero");
		return result;
	}
	//n > order >= 1

	const K n_minus_order = n - order;
	const K n_plus_order = n + order;

	std::vector<T> T_n(
		n_plus_order, 
		convertWithPrec<T>(0.0, currentPrecision)
	);

	T a_n = convertWithPrec<T>(0.0, currentPrecision);
	T a_n_plus_1 = convertWithPrec<T>(0.0, currentPrecision);
	T tmp = convertWithPrec<T>(0.0, currentPrecision);

	for (K i = n_minus_order + static_cast<K>(1); i <= n_plus_order - static_cast<K>(1); ++i) // if we got to this branch then we know that n >= order - see previous branches  int -> K
	{
		a_n = data.an.at(i);
		a_n_plus_1 = data.an.at(i + static_cast<K>(1));
		tmp = -a_n_plus_1 * a_n_plus_1;

		// For theory, see: Shanks (1955), Eq. (12) - Higher order transformation
		// eₖ(Sₙ) = eₖ₋₁(Sₙ) + [eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ)] / [1 - (eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ))/(eₖ₋₁(Sₙ₊₂) - eₖ₋₁(Sₙ₊₁))]
		T_n[i] = fma(
			a_n * a_n_plus_1,
			(a_n + a_n_plus_1) / (fma(a_n, a_n, tmp) - fma(a_n_plus_1, a_n_plus_1, tmp)),
			data.Sn.at(i)
		);
	}

	std::vector<T> T_n_plus_1(
		n + order, 
		convertWithPrec<T>(0.0, currentPrecision)
	);

	T a = convertWithPrec<T>(0.0, currentPrecision);
	T b = convertWithPrec<T>(0.0, currentPrecision);
	T c = convertWithPrec<T>(0.0, currentPrecision);;

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
 * @brief Shanks transformation for alternating series class template.
 *
 * @tparam T Floating-point type for series elements (must satisfy Accepted)
 *           Represents numerical precision (float, double, long double)
 *           Used for all arithmetic operations and intermediate calculations
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting terms, indexing operations, and transformation order
 * @tparam series_templ Type of series object to accelerate. Must provide:
 *           - T operator()(K n) const: returns the n-th series term a_n
 *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
 *           Series terms should alternate in sign for optimal performance
 */
template <AcceptedLike T, UnsignedIntLike K>
class shanks_transform_alternating : public series_acceleration<T, K>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Shanks transformation for alternating series.
	 * @authors Bolshakov M.P.
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 *        Series terms should alternate in sign for optimal performance
	 */
	explicit shanks_transform_alternating() : series_acceleration<T, K>("shanks alternating") {};

	/**
	 * @brief Shanks transformation for alternating series function.
	 * @authors Bolshakov M.P., Pashkov B.B.
	 * @param n The number of terms in the partial sum to use for transformation
	 *        Valid values: n >= order > 0, n > 0
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (number of recursive applications)
	 *        Valid values: order >= 0 (order=0 returns untransformed partial sum)
	 *        Higher orders provide more aggressive acceleration but may be less stable
	 * @return The accelerated partial sum after Shanks transformation
	 * @throws std::overflow_error if division by zero or numerical instability occurs
	 */
	T operator()(const K n, 
        const K order,
		const SeriesResult<T>& data,
        const K offset = static_cast<K>(0)
	) const override;

};

template <AcceptedLike T, UnsignedIntLike K>
T shanks_transform_alternating<T, K>::operator()(
	const K n, 
    const K order,
	const SeriesResult<T>& data,
    const K offset
) const {

    K required_size = order + offset + static_cast<K>(1);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("Sn or an is smaller than required to calculate theta_{" + to_string(order) + "}^{" + to_string(n) + "}");
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;
	using std::fma;

	size_t currentPrecision = std::max(
        series_acceleration<T, K>::define_precision(data.Sn[0]), 
        series_acceleration<T, K>::define_precision(data.an[0])
    );

	if (order == static_cast<K>(1)) [[unlikely]]
	{
		T a_n = data.an.at(n);
		T a_n_plus_1 = data.an.at(n + static_cast<K>(1));
		T result = convertWithPrec<T>(0.0, currentPrecision);

		// For theory, see: Senhadji (2001), Section 3.2 - Alternating series case
		// For alternating series: e₁(Sₙ) = Sₙ + (aₙaₙ₊₁)/(aₙ - aₙ₊₁)
		result += fma(
			a_n * a_n_plus_1,
			static_cast<T>(1) / (a_n - a_n_plus_1),
			data.Sn.at(n)
		);

		if (!isfinite(result))
			throw std::overflow_error("division by zero");

		return result;
	}
	//n > order >= 1

	const K n_minus_order = n - order;
	const K n_minus_order1 = n_minus_order + static_cast<K>(1);
	const K n_plus_order = n + order;

	std::vector<T> T_n(
		n_plus_order, 
		convertWithPrec<T>(0.0, currentPrecision)
	);

	T a_n = convertWithPrec<T>(0.0, currentPrecision);
	T a_n_plus_1 = convertWithPrec<T>(0.0, currentPrecision);

	for (K i = n_minus_order1; i <= n_plus_order - static_cast<K>(1); ++i) // if we got to this branch then we know that n >= order - see previous branches int->K
	{
		a_n = data.an.at(i);
		a_n_plus_1 = data.an.at(i + static_cast<K>(1));

		// For theory, see: Senhadji (2001), Section 3.2 - Alternating series case
		// e₁(Sᵢ) = Sᵢ + (aᵢaᵢ₊₁)/(aᵢ - aᵢ₊₁)
		T_n[i] = fma(
			a_n * a_n_plus_1,
			static_cast<T>(1) / (a_n - a_n_plus_1),
			data.Sn.at(n)
		);
	}

	std::vector<T> T_n_plus_1(
		n_plus_order, 
		convertWithPrec<T>(0.0, currentPrecision)
	);

	T a = convertWithPrec<T>(0.0, currentPrecision);
	T b = convertWithPrec<T>(0.0, currentPrecision);
	T c = convertWithPrec<T>(0.0, currentPrecision);

	for (K j = static_cast<K>(2); j <= order; ++j) {
		for (K i = n_minus_order + j; i <= n_plus_order - j; ++i) {

			a = T_n[i];
			b = T_n[i - static_cast<K>(1)];
			c = T_n[i + static_cast<K>(1)];

			// For theory, see: Brezinski et al. (2010), Section 3 - Multistep transformation
			// Higher order transformation for alternating series
			// eₖ(Sₙ) = eₖ₋₁(Sₙ) + [eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ)] / [1 - (eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ))/(eₖ₋₁(Sₙ₊₂) - eₖ₋₁(Sₙ₊₁))]
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
