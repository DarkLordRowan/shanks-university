#ifndef SHANKS_ALGTERNATING_ALGORITHM_HPP
#define SHANKS_ALGTERNATING_ALGORITHM_HPP
#pragma once

/**
 * @file shanks_alternating_algorithm.hpp
 * @brief This file contains the definition of the Shanks transformation specialized for alternating series.
 * @authors Bolshakov M.P., Naumov A.U., Lykov D.S., Kreynin R.G.
 */

#include "series_acceleration.hpp"

namespace shanks{ namespace algos{

/**
 * @brief Shanks transformation for alternating series class template.
 *
 * This class provides an optimized implementation of the Shanks transformation specifically
 * tailored for alternating series. It utilizes simplified formulas that are more numerically
 * stable for such series compared to the general determinant-based approach.
 *
 * @authors Bolshakov M.P., Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam T Floating-point type for series elements (must satisfy AcceptedLike).
 *           Represents numerical precision (float, double, long double, or arbitrary precision).
 * @tparam K Unsigned integral type for indices and order (must satisfy UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class shanks_transform_alternating : public series_acceleration<T, K>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Shanks transformation for alternating series.
	 * @authors Bolshakov M.P.
	*/
	explicit shanks_transform_alternating() : series_acceleration<T, K>("shanks alternating") {};

	/**
	 * @brief Executes the Shanks transformation for alternating series.
	 *
	 * Computes the accelerated partial sum using a recursive or simplified formula
	 * appropriate for alternating sequences.
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param n The number of terms in the partial sum to use for transformation
	 *        Valid values: n >= order > 0, n > 0
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (number of recursive applications)
	 *        Valid values: order >= 0 (order=0 returns untransformed partial sum)
	 *        Higher orders provide more aggressive acceleration but may be less stable
	 * @param data series_result<T> struct containing necessary information for algorithm
	 * @return T The accelerated partial sum result.
	 * @throws std::out_of_range if the input vectors Sn or an are smaller than required.
	 * @throws std::invalid_argument if n is less than the requested order.
	 * @throws std::overflow_error if division by zero or non-finite result occurs.
	 */
	T operator()(
        const K n,
    	const K order,
		const series_result<T>& data
	) const override;

};

template <AcceptedLike T, UnsignedIntLike K>
T shanks_transform_alternating<T, K>::operator()(
	const K n,
    const K order,
	const series_result<T>& data
) const {

    // Ensure we have enough terms in both Sn and an vectors
    const K required_size = order + n + static_cast<K>(1);
    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for alt_shanks_{" + utils::to_string(order) + "}^{" + utils::to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + utils::to_string(required_size));
	}

    // Trivial case: order 0 returns the current partial sum
    if (order == static_cast<K>(0)) return data.Sn.at(n);

    // Special case: first-order Shanks transformation
	if (order == static_cast<K>(1)) [[unlikely]]
	{

		T result = utils::cast<T>(0.0);

		// For theory, see: Senhadji (2001), Section 3.2 - Alternating series case
		// For alternating series: e₁(Sₙ) = Sₙ + (aₙaₙ₊₁)/(aₙ - aₙ₊₁)
		result += utils::fma(
			data.an.at(n) * data.an.at(n + static_cast<K>(1)),
			utils::cast<T>(1) / (data.an.at(n) - data.an.at(n + static_cast<K>(1))),
			data.Sn.at(n)
		);

		if(!utils::isfinite(result)) throw std::overflow_error("division by zero");

		return result;
	}
	//n > order >= 1
	if (n < order){
		throw std::invalid_argument("n is less than order");
	}

	std::vector<T> T_n(
		n + order,
		utils::cast<T>(0.0)
	);

	for (K i = n - order + static_cast<K>(1); i <= n + order - static_cast<K>(1); ++i) // if we got to this branch then we know that n >= order - see previous branches int->K
	{

		// For theory, see: Senhadji (2001), Section 3.2 - Alternating series case
		// e₁(Sᵢ) = Sᵢ + (aᵢaᵢ₊₁)/(aᵢ - aᵢ₊₁)
		T_n[i] = utils::fma(
			data.an.at(i) * data.an.at(i + static_cast<K>(1)),
			utils::cast<T>(1) / (data.an.at(i) - data.an.at(i + static_cast<K>(1))),
			data.Sn.at(n)
		);
	}

	std::vector<T> T_n_plus_1(
		n + order,
		utils::cast<T>(0.0)
	);

	T a = utils::cast<T>(0.0);
	T b = utils::cast<T>(0.0);
	T c = utils::cast<T>(0.0);

	for (K j = static_cast<K>(2); j <= order; ++j) {
		for (K i = n - order + j; i <= n + order - j; ++i) {

			a = T_n[i];
			b = T_n[i - static_cast<K>(1)];
			c = T_n[i + static_cast<K>(1)];

			// For theory, see: Brezinski et al. (2010), Section 3 - Multistep transformation
			// Higher order transformation for alternating series
			// eₖ(Sₙ) = eₖ₋₁(Sₙ) + [eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ)] / [1 - (eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ))/(eₖ₋₁(Sₙ₊₂) - eₖ₋₁(Sₙ₊₁))]
			T_n_plus_1[i] = utils::fma(
				utils::fma(a, c + b - a, -b * c),
				utils::cast<T>(1) / (utils::cast<T>(2) * a - b - c),
				a
			);
		}
		T_n = T_n_plus_1;
	}

	if(!utils::isfinite(T_n[n])) throw std::overflow_error("division by zero");

	return T_n[n];

}

} //namespace shanks::algos
} //namespace shanks

#endif
