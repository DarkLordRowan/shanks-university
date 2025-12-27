#ifndef LEVIN_SIDI_S_ALGORITHM_HPP
#define LEVIN_SIDI_S_ALGORITHM_HPP
#pragma once
/**
 * @file levin_sidi_s_algorithm.hpp
 * @brief Contains implementation of Drummond's D-transformation (Levin-Sidi S-transformation)
 *
 * For theory, see:
 * Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications.
 *   Cambridge University Press. (Chapter 8, pp. 57-58)
 * Sidi, A. (2003). A new class of nonlinear transformations for accelerating the convergence
 *   of infinite integrals and series. arXiv:math/0306302.
 */

#include "remainders.hpp"
#include "series_acceleration.hpp"

#include <memory>


 /**
  * @brief Levin-Sidi S-transformation class template (Drummond's D-transformation).
  *
  * @authors Naumov A.
  *
  * This class implements the Levin-Sidi S-transformation,
  * which is particularly effective for series with specific asymptotic behaviors. The transformation
  * uses Pochhammer symbols and can be computed using either direct formulas or recurrence relations.
  *
  * References:
  * - Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications.
  * - Sidi, A. (2003). A new class of nonlinear transformations. arXiv:math/0306302.
  *
  * @tparam T Floating-point type for series elements (must satisfy Accepted)
  *           Represents numerical precision (float, double, long double)
  * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
  *           Used for counting and indexing operations
  */
template<AcceptedLike T, UnsignedIntLike K>
class levin_sidi_s_algorithm final : public series_acceleration<T, K> {
protected:

    using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval

    float_type beta_in_use = utils::cast<float_type>(1.0);       /**< Positive real parameter (β > 0). Default value is 1.0.            */
    std::unique_ptr<const transform_base<T, K>> remainder;         /**< Pointer to remainder transformation object                        */
    bool use_recurrent_formula = false;                            /**< Flag to use recurrence formulas (true) or direct formulas (false) */
    remainder_type remainder_type_in_use = remainder_type::u_type; /**< Type of Levin transformation variant (u, t, v, t~, v~)            */

    /**
     * @brief Computes the S-transformation using direct summation formulas.
     *
     * For theory, see: Sidi (2003, arXiv:math/0306302), pp. 57-58, Eqs. (8.2)-(8.7) ([https://arxiv.org/pdf/math/0306302.pdf])
     * General form: S_{k,n} =  [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} S_{n+j}/R_{n+j}] /
     *                          [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} 1/R_{n+j}]
     *
     * @param n Starting index for the transformation
     * @param order Order of transformation (k value)
     * @return Accelerated sum estimate S_{k,n}
    */
    inline T calc_result(
        const K n, 
        const K order, 
        const series_result<T>& data
    ) const;

    /**
     * @brief Computes the S-transformation using recurrence formulas.
     *
     * For theory, see: Sidi (2003, arXiv:math/0306302), pp. 57-58, Eqs. (8.3)-(8.5) ([https://arxiv.org/pdf/math/0306302.pdf])
     * Recursive implementation for better numerical stability in some cases.
     *
     * @param n Starting index for the transformation
     * @param order Order of transformation (k value)
     * @return Accelerated sum estimate S_{k,n}
    */
    inline T calc_result_rec(
        const K n, 
        const K order, 
        const series_result<T>& data
    ) const;

public:

    /**
     * @brief Parameterized constructor to initialize the Levin-Sidi S-transformation.
     *
     * @param variant Type of remainder transformation to use
     *        Valid values: u_type, t_type, v_type, t_wave_type, v_wave_type
     *        Determines the remainder estimate R_n used in the transformation
     * @param use_recurrent_formula Flag to use recurrence formulas instead of direct summation
     *        true: use recursive implementation, false: use direct summation
     * @param parameter Positive real parameter β (must be > 0)
     *        Default value: 1.0. Affects the Pochhammer symbol terms in the transformation.
     *        For theory, see: Sidi (2003, arXiv:math/0306302), p. 39
    */
    explicit levin_sidi_s_algorithm(
        remainder_type remainder_type_in_use = remainder_type::u_type,
        bool use_recurrent_formula = false,
        const float_type& beta_to_use = utils::cast<float_type>(1.0)
    ) : series_acceleration<T, K>(), use_recurrent_formula(use_recurrent_formula){
        // parameter is "beta" parameter
        // beta must be nonzero positive real number
        // beta = 1 is default
        // check parameter else default
        update_beta(beta_to_use);
        update_type(remainder_type_in_use);
    }


    /**
     * @brief Implementation of Levin-Sidi S-transformation for series acceleration.
     *
     * Computes the accelerated sum using the S-transformation (Drummond's D-transformation),
     * which is particularly effective for series with specific asymptotic behaviors.
     *
     * For theory, see:
     * - General framework: Sidi (2003, arXiv:math/0306302), Eqs. (8.2)-(8.7)
     * - Convergence properties: Sidi (2003, Practical Extrapolation Methods), Chapter 8
     *
     * @param n The starting index for the transformation
     *        Valid values: n >= 0
     * @param order The order of transformation (k value)
     *        Valid values: order >= 0
     * @return The accelerated partial sum after S-transformation
     * @throws std::overflow_error if division by zero or numerical instability occurs
    */
    T operator()(
        const K n, 
        const K order, 
        const series_result<T>& data
    ) const override;

    /**
     * @brief 
     * @param new_beta 
    */
    void update_beta(const float_type& new_beta){ beta_in_use = (new_beta > utils::cast<float_type>(0.0) ?  new_beta : utils::cast<float_type>(1.0)); }

    /**
     * @brief 
     * @param remainder_type_to_use 
    */
    void update_type(const remainder_type remainder_type_to_use){

		remainder_type_in_use = remainder_type_to_use;

		switch(remainder_type_to_use){
        	case remainder_type::u_type 	: { remainder.reset(new u_transform<T, K>()	   ); break; }
        	case remainder_type::t_type 	: { remainder.reset(new t_transform<T, K>()	   ); break; }
        	case remainder_type::v_type 	: { remainder.reset(new v_transform<T, K>()	   ); break; }
        	case remainder_type::t_wave_type: { remainder.reset(new t_wave_transform<T, K>()); break; }
        	case remainder_type::v_wave_type: { remainder.reset(new v_wave_transform<T, K>()); break; }
        	default:{
				remainder_type_in_use = remainder_type::u_type;
        	    remainder.reset(new u_transform<T, K>()); // Default to u-variant
			}
		}
	}

    /**
     * @brief Get the name object
     * 
     * @return std::string 
    */
	std::string get_name() override {

		series_acceleration<T, K>::acceleration_name = (use_recurrent_formula ? "recurrent " : "");
		series_acceleration<T, K>::acceleration_name += "levin sidi s algorithm ";
		switch(remainder_type_in_use){
			case remainder_type::u_type 	: { series_acceleration<T, K>::acceleration_name += "with u-variant "; 		break; }
			case remainder_type::t_type 	: { series_acceleration<T, K>::acceleration_name += "with t-variant "; 		break; }
			case remainder_type::v_type 	: { series_acceleration<T, K>::acceleration_name += "with v-variant "; 		break; }
			case remainder_type::t_wave_type: { series_acceleration<T, K>::acceleration_name += "with t-wave-variant "; break; }
			case remainder_type::v_wave_type: { series_acceleration<T, K>::acceleration_name += "with v-wave-variant "; break; }
		}
		series_acceleration<T, K>::acceleration_name += "and beta = " + utils::to_string(beta_in_use);

        return series_acceleration<T, K>::acceleration_name;
	}

};

template<AcceptedLike T, UnsignedIntLike K>
inline T levin_sidi_s_algorithm<T, K>::calc_result(
    const K n, 
    const K order, 
    const series_result<T>& data
) const {

    T numerator, denominator, rest;
    float_type up_pochamer, down_pochamer;
    numerator = denominator = rest = utils::cast<T>(0.0);
    up_pochamer = down_pochamer = utils::cast<float_type>(0.0);

    if constexpr (is_precisable<T>::value){
        const size_t precision = std::max(utils::get_precision(data.Sn[0]), utils::get_precision(data.an[0]));
        utils::set_precision(precision, numerator, denominator, rest, up_pochamer, down_pochamer);
    }

    // For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (8.2)
    // S_{k,n} = [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} S_{n+j}/R_{n+j}] /
    //           [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} 1/R_{n+j}]
    for (K j = static_cast<K>(0); j <= order; ++j){

        // Compute (-1)^j * C(k,j)
        rest += -rest + utils::cast<T>(1.0);
        rest *= utils::minus_one_raised_to_power_n<T,K>(j);
        rest *= utils::cast<T>((utils::binomial_coefficient<K>(order, j)));

        // Compute Pochhammer symbols: (β+n+j)_{k-1} and (β+n+k)_{k-1}
        up_pochamer = down_pochamer = utils::cast<float_type>(1.0);

        //up_pochamer   (beta + n + j)_(order - 1)     = (beta + n + j)(beta + n + j + 1)...(beta + n + j + order - 2)
        //down_pochamer (beta + n + order)_(order - 1) = (beta + n + order)(beta + n + order + 1)...(beta + n + order + oreder - 2)

        // (β+n+j)_{k-1} = ∏_{i=0}^{k-2} (β+n+j+i)
        // (β+n+k)_{k-1} = ∏_{i=0}^{k-2} (β+n+k+i)
        for (K i = static_cast<K>(0); i < order - static_cast<K>(1); ++i){
            up_pochamer   *= (beta_in_use + utils::cast<float_type>((n + j     + i)));
            down_pochamer *= (beta_in_use + utils::cast<float_type>((n + order + i)));
        }


        rest *= (up_pochamer / down_pochamer);  // Multiply by Pochhammer ratio
        rest *= remainder->operator()(
            n + j,        // Multiply by remainder term 1/R_{n+j}
            n + j,
            data.an,
            (remainder_type_in_use == remainder_type::u_type ? utils::cast<T>(beta_in_use) : utils::cast<T>(1.0))
        );

        // Accumulate numerator and denominator
        numerator   += rest * data.Sn.at(n + j);
        denominator += rest;
    }

    numerator /= denominator;

    return numerator;
}

template<AcceptedLike T, UnsignedIntLike K>
inline T levin_sidi_s_algorithm<T, K>::calc_result_rec(
    const K n, 
    const K order, 
    const series_result<T>& data
) const {

    // For theory, see: Sidi (2003, arXiv:math/0306302), Eqs. (8.3)-(8.5)
    // Recursive implementation using the E-algorithm scheme
    std::vector<T>   Num(order + static_cast<K>(1),  utils::cast<T>(0.0));
    std::vector<T> Denom(order + static_cast<K>(1),  utils::cast<T>(0.0));

    float_type scale1, scale2;
    scale1 = scale2 = utils::cast<float_type>(0.0);

    if constexpr (is_precisable<T>::value){
        const size_t precision = std::max(utils::get_precision(data.Sn[0]), utils::get_precision(data.an[0]));
        utils::set_vec_precision(Num, precision);
        utils::set_vec_precision(Denom, precision);
        utils::set_precision(precision, scale1, scale2);
    }

    // Initialize base values: E_0^{(n)} = S_n, g_0^{(n)} = 1/R_n
    for (K i = static_cast<K>(0); i <= order; ++i){

        Denom[i] += remainder->operator()(
            n + i,
            n + i,
            data.an,
            (remainder_type_in_use == remainder_type::u_type ? utils::cast<T>(beta_in_use) : utils::cast<T>(1.0))
        );

        Num[i] += data.Sn.at(n + i) * Denom[i];

    }

    // Recursive computation using the E-algorithm recurrence
    for (K i = static_cast<K>(1); i <= order; ++i)
        for(K j = static_cast<K>(0); j <= order - i; ++j){

            // i ~ k from formula
            // j ~ n from formula

            // For theory, see: Sidi (2003), Eqs. (8.4)-(8.5)
            // Compute scaling factors based on Pochhammer symbol ratios

            scale1 = beta_in_use + utils::cast<float_type>((n + i + j));
            scale1*= (scale1 + utils::cast<float_type>(1.0));

            scale2 = scale1 + utils::cast<float_type>((n + i));
            scale2*= (scale2 + utils::cast<float_type>(1.0));

            // Apply recurrence: E_k^{(n)} = E_{k-1}^{(n)} - g_{k-1,k}^{(n)} * ΔE_{k-1}^{(n)} / Δg_{k-1,k}^{(n)}
            Denom[j] = utils::fma(utils::cast<T>(-scale1),Denom[j]/utils::cast<T>(scale2),Denom[j+static_cast<K>(1)]);
              Num[j] = utils::fma(utils::cast<T>(-scale1),  Num[j]/utils::cast<T>(scale2),  Num[j+static_cast<K>(1)]);
        }

    Num[0] /= Denom[0];

    return Num[0];
}

template<AcceptedLike T, UnsignedIntLike K>
T levin_sidi_s_algorithm<T, K>::operator()(
    const K n, 
    const K order, 
    const series_result<T>& data
) const{

    const K required_size = n + order +  static_cast<K>(1) + static_cast<K>(
		remainder_type_in_use == remainder_type::t_wave_type ||
		remainder_type_in_use == remainder_type::v_type ||
		remainder_type_in_use == remainder_type::v_wave_type
	);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for S_{" + utils::to_string(order) + "}^{" + utils::to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + utils::to_string(required_size));
	}

    if (order == static_cast<K>(0)) return data.Sn.at(n);

    const T result = (use_recurrent_formula ? calc_result_rec(n,order, data) : calc_result(n, order, data));

    if(!utils::isfinite(result)) throw std::overflow_error("division by zero");

    return result;
}

#endif