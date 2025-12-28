#ifndef SHANKS_ALGTERNATING_ALGORITHM_HPP
#define SHANKS_ALGTERNATING_ALGORITHM_HPP
#pragma once

#include "series_acceleration.hpp"

namespace shanks{ namespace algos{

/**
 * @brief Shanks transformation for alternating series class template.
 *
 * @tparam T Floating-point type for series elements (must satisfy Accepted)
 *           Represents numerical precision (float, double, long double)
 *           Used for all arithmetic operations and intermediate calculations
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting terms, indexing operations, and transformation order
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
	 * @brief Shanks transformation for alternating series function.
	 * @authors Bolshakov M.P., Pashkov B.B.
	 * @param n The number of terms in the partial sum to use for transformation
	 *        Valid values: n >= order > 0, n > 0
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (number of recursive applications)
	 *        Valid values: order >= 0 (order=0 returns untransformed partial sum)
	 *        Higher orders provide more aggressive acceleration but may be less stable
	 * @param data series_result<T> struct containing necessary information for algorithm
	 * @return The accelerated partial sum after Shanks transformation
	 * @throws std::overflow_error if division by zero or numerical instability occurs
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

    const K required_size = order + n + static_cast<K>(1);
    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for alt_shanks_{" + utils::to_string(order) + "}^{" + utils::to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + utils::to_string(required_size));
	}

    if (order == static_cast<K>(0)) return data.Sn.at(n);

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