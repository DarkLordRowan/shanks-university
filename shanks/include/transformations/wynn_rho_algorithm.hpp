#ifndef WYNN_RHO_ALGORITHM_HPP
#define WYNN_RHO_ALGORITHM_HPP
#pragma once
/**
 * @file wynn_rho_algorithm.hpp
 * @brief This file contains the declaration of the Wynn's Rho Algorithm class.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * For theory, see:
 * Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 * Wynn, P. (1966). Upon a second-order continued-fraction transformation for accelerating convergence.
 * Osada, N. (1993). Acceleration Methods for Slowly Convergent Sequences and Their Applications.
 */


#include "series_acceleration.hpp"
#include "wynn_numerators.hpp"

#include <memory>

namespace shanks{ namespace algos{

 /**
  * @brief Wynn's Rho Algorithm class template for sequence acceleration.
  *
  * This class implements Wynn's rho algorithm, which is particularly effective
  * for accelerating logarithmically convergent sequences. The algorithm uses
  * continued fraction representations and reciprocal differences. Several variants
  * are supported through different numerator computation strategies.
  *
  * References:
  * - Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
  * - Wynn, P. (1966). Upon a second-order continued-fraction transformation for accelerating convergence.
  * - Osada, N. (1993). Acceleration Methods for Slowly Convergent Sequences and Their Applications.
  * - Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications.
  *
  * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
  * @tparam T Floating-point type for series elements (must satisfy Accepted)
  *           Represents numerical precision (float, double, long double)
  *           Used for all mathematical computations and storage
  * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
  *           Used for counting terms, indexing operations, and order specification
  *           Valid values: K >= 0, typically size_t or unsigned int
  */
template <AcceptedLike T, UnsignedIntLike K>
class wynn_rho_algorithm final : public series_acceleration<T, K>
{
protected:

	using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval, represents type for real numbers

	/// Strategy object for numerator computation.
	std::unique_ptr<const shanks::numerators::numerator_base<T, K>> numerator;

	/// Gamma parameter, used in generalized and gamma-rho variants.
	float_type gamma_in_use;

	/// Rho parameter, used specifically in the gamma-rho variant.
	float_type rho_in_use;

	/// The current numerator type variant in use.
	shanks::numerators::numerator_type numerator_type_in_use{shanks::numerators::numerator_type::rho_type};


public:

	/**
	 * @brief Parameterized constructor to initialize the Rho Wynn Algorithm.
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param numerator_type_to_use The variant of numerator logic to apply (rho, generalized, or gamma-rho).
	 * @param gamma_to_use Initial value for the gamma parameter.
	 * @param rho_to_use Initial value for the rho parameter.
	 */
	explicit wynn_rho_algorithm(
		shanks::numerators::numerator_type numerator_type_to_use = shanks::numerators::numerator_type::rho_type,
		const float_type& gamma_to_use = utils::cast<float_type>(-1.0),
		const float_type& rho_to_use   = utils::cast<float_type>(1.0)
	) : series_acceleration<T, K>() {
		update_gamma(gamma_to_use);
		update_rho(rho_to_use);
		update_numerator(numerator_type_to_use);
	};

	/**
	 * @brief Virtual destructor.
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 */
	~wynn_rho_algorithm() = default;

	/**
	 * @brief Wynn's rho algorithm transformation.
	 *
	 * Computes the accelerated partial sum using Wynn's rho algorithm.
	 * The algorithm is particularly effective for logarithmically convergent
	 * sequences and uses continued fraction representations for acceleration.
	 *
	 * For theory, see: Wynn (1966), Main Theorem
	 * ρ₂ₖ⁽ⁿ⁾ provides accelerated convergence for sequences satisfying
	 * S_n ~ S + Σ c_k n^{-k} as n → ∞
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param n The starting index for partial sums
	 *        Valid values: n >= 0, determines the starting point of transformation
	 * @param order The order of transformation
	 *        Valid values: order >= 0 and must be even (order % 2 == 0)
	 *        Higher orders use more terms but may provide better acceleration
	 * @param data series_result<T> struct containing necessary information for algorithm
	 * @return T The accelerated partial sum result.
	 * @throws std::out_of_range if the input data vectors are too small for the requested order.
	 * @throws std::overflow_error if division by zero or non-finite result occurs.
	 */
	T operator()(
		const K n,
        const K order,
        const series_result<T>& data
	) const override;

	/**
	 * @brief Updates the gamma parameter.
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param new_gamma The new value for gamma.
	*/
	void update_gamma(const float_type& new_gamma) { gamma_in_use = new_gamma;}

	/**
	 * @brief Updates the rho parameter.
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param new_rho The new value for rho.
	*/
	void update_rho(const float_type& new_rho) {rho_in_use = new_rho; }

	/**
	 * @brief Changes the numerator computation variant.
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param numerator_type_to_use The new numerator variant (rho, generalized, or gamma-rho).
	*/
	void update_numerator(const shanks::numerators::numerator_type numerator_type_to_use){

		numerator_type_in_use = numerator_type_to_use;

		// Re-instantiate the numerator logic based on the requested variant
		switch(numerator_type_to_use){
        	case shanks::numerators::numerator_type::rho_type 		  : { numerator.reset(new shanks::numerators::rho_transform<T, K>()	    ); break; }
        	case shanks::numerators::numerator_type::generalized_type : { numerator.reset(new shanks::numerators::generilized_transform<T, K>()); break; }
        	case shanks::numerators::numerator_type::gamma_rho_type   : { numerator.reset(new shanks::numerators::gamma_rho_transform<T, K>()	); break; }
        	default:{
				numerator_type_in_use = shanks::numerators::numerator_type::rho_type;
        	    numerator.reset(new shanks::numerators::rho_transform<T, K>());
			}
		}
	}

	/**
	 * @brief Returns the descriptive name of the currently active Rho variant.
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @return std::string The name and current parameter configuration of the algorithm.
	*/
	std::string get_name() override {

		series_acceleration<T, K>::acceleration_name = "wynn rho algorithm ";
		switch(numerator_type_in_use){
			case shanks::numerators::numerator_type::rho_type 			: { series_acceleration<T, K>::acceleration_name += "with rho-numerator "; break; }
			case shanks::numerators::numerator_type::generalized_type 	: { series_acceleration<T, K>::acceleration_name += "with generalized-numerator "; break; }
			case shanks::numerators::numerator_type::gamma_rho_type 	: { series_acceleration<T, K>::acceleration_name += "with v-numerator "; break; }
		}
		series_acceleration<T, K>::acceleration_name += ", gamma = " + utils::to_string(gamma_in_use);
		series_acceleration<T, K>::acceleration_name += ", rho = " + utils::to_string(rho_in_use);

		return series_acceleration<T, K>::acceleration_name;
	}

};

template <AcceptedLike T, UnsignedIntLike K>
inline T wynn_rho_algorithm<T, K>::operator()(
	const K n,
    const K order,
    const series_result<T>& data
) const {

    // Ensure we have enough data points to compute the transformation
	const K required_size = n + order + static_cast<K>(1) + order * static_cast<K>(numerator_type_in_use == shanks::numerators::numerator_type::rho_type);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for wynn_rho_{" + utils::to_string(order) + "}^{" + utils::to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + utils::to_string(required_size));
	}
    // For theory, see: Brezinski (1977), Chapter 4, Eq. (4.10)
    // Base cases: return partial sum for n=0 or order=0
    if (order == static_cast<K>(0))return data.Sn.at(n);

	const K base_size = order + static_cast<K>(1);

    std::vector<T> rho_odd(base_size, utils::cast<T>(0.0)); // vector for theta_(2n + 1)
    std::vector<T> rho_even(base_size, utils::cast<T>(0.0)); //vector for theta_(2n), in the beginning it is theta_(-1) which is zero for all i

	T delta; //temporary varaible
	delta = utils::cast<T>(0.0);

	//setting precision in case the type is able to have one
	if constexpr (is_precisable<T>::value){
		const size_t precision = std::max(utils::get_precision(data.Sn[0]), utils::get_precision(data.an[0]));
		utils::set_vec_precision<T>(rho_odd, precision);
		utils::set_vec_precision<T>(rho_even, precision);
	}

    // init theta_(0)
    for(K j = static_cast<K>(0); j < base_size; ++j) {
        rho_even[j] = data.Sn.at(n + j);
	}

    K j1, j2;

    for(K level = static_cast<K>(1); level <= order / static_cast<K>(2); ++level){

		// transform odd vector
        for(K j = static_cast<K>(0); j < base_size - level; ++j){

            j1 = j + static_cast<K>(1);

            delta = rho_even[j1] - rho_even[j];

			rho_odd[j] = rho_odd[j1] + numerator->operator()(
				n + j,
				level * static_cast<K>(2) - static_cast<K>(1),
				data.an,
				gamma_in_use,
				rho_in_use
			) / delta;

        }


		// transform even vector
        for(K j = static_cast<K>(0); j < base_size - level; ++j){

            j1 = j + static_cast<K>(1);

            delta = rho_odd[j1] - rho_odd[j];

			rho_even[j] = rho_even[j1] + numerator->operator()(
				n + j,
				level * static_cast<K>(2),
				data.an,
				gamma_in_use,
				rho_in_use
			) / delta;

        }

    }

    if(!utils::isfinite(rho_even[0])) throw std::overflow_error("division by zero");

    return rho_even[0];
}

} //namespace shanks::algos
} //namespace shanks

#endif
