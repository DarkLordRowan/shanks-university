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

#pragma once

#include "../remainders.hpp"
#include "../series_acceleration.hpp"

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
  * @tparam series_templ Type of series object to accelerate. Must provide:
  *           - T operator()(K n) const: returns the n-th series term a_n
  *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
  *           - T minus_one_raised_to_power_n(K j) const: returns (-1)^j
  *           - T binomial_coefficient(T n, K k) const: returns binomial coefficient C(n, k)
  */
template<AcceptedLike T, UnsignedIntLike K>
class levin_sidi_s_algorithm final : public series_acceleration<T, K> {
protected:

    T beta_in_use;                                                 ///< Positive real parameter (β > 0). Default value is 1.0.
    std::unique_ptr<const transform_base<T, K>> remainder;  ///< Pointer to remainder transformation object
    bool use_recurrent_formula = false;                            ///< Flag to use recurrence formulas (true) or direct formulas (false)
    remainder_type remainder_type_in_use = remainder_type::u_type;     ///< Type of Levin transformation variant (u, t, v, t~, v~)

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
	* @brief Function to calculate S-tranformation using recurrence formula.
	* @param n The partial sum number (S_n) from which the calculations will be done
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/

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
     * @param series The series class object to be accelerated
     *        Must be a valid object implementing the required series interface
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
        const T& beta_to_use = static_cast<T>(1)
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

    void update_beta(const T& new_beta){ beta_in_use = (new_beta > static_cast<T>(0) ?  new_beta : static_cast<T>(1)); }

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

	std::string get_name() {

		using std::to_string;

		series_acceleration<T, K>::acceleration_name = (use_recurrent_formula ? "recurrent " : "");
		series_acceleration<T, K>::acceleration_name += "levin sidi s algorithm ";
		switch(remainder_type_in_use){
			case remainder_type::u_type 	: { series_acceleration<T, K>::acceleration_name += "with u-variant "; 		break; }
			case remainder_type::t_type 	: { series_acceleration<T, K>::acceleration_name += "with t-variant "; 		break; }
			case remainder_type::v_type 	: { series_acceleration<T, K>::acceleration_name += "with v-variant "; 		break; }
			case remainder_type::t_wave_type: { series_acceleration<T, K>::acceleration_name += "with t-wave-variant "; break; }
			case remainder_type::v_wave_type: { series_acceleration<T, K>::acceleration_name += "with v-wave-variant "; break; }
		}
		series_acceleration<T, K>::acceleration_name += "and beta = " + to_string(beta_in_use);

        return series_acceleration<T, K>::acceleration_name;
	}

};

template<AcceptedLike T, UnsignedIntLike K>
inline T levin_sidi_s_algorithm<T, K>::calc_result(
    const K n, 
    const K order, 
    const series_result<T>& data
) const {

    using std::isfinite;

    T numerator   = static_cast<T>(0.0);
    T denominator = static_cast<T>(0.0);
    T rest;
    T up_pochamer, down_pochamer;

    // For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (8.2)
    // S_{k,n} = [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} S_{n+j}/R_{n+j}] /
    //           [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} 1/R_{n+j}]
    for (K j = static_cast<K>(0); j <= order; ++j){

        // Compute (-1)^j * C(k,j)
        rest  = static_cast<T>(1.0);
        rest *= minus_one_raised_to_power_n<T,K>(j);
        rest *= static_cast<T>(binomial_coefficient<K>(order, j));

        // Compute Pochhammer symbols: (β+n+j)_{k-1} and (β+n+k)_{k-1}
        up_pochamer = down_pochamer = static_cast<T>(1.0);
        //up_pochamer   (beta + n + j)_(order - 1)     = (beta + n + j)(beta + n + j + 1)...(beta + n + j + order - 2)
        //down_pochamer (beta + n + order)_(order - 1) = (beta + n + order)(beta + n + order + 1)...(beta + n + order + oreder - 2)

        // (β+n+j)_{k-1} = ∏_{i=0}^{k-2} (β+n+j+i)
        // (β+n+k)_{k-1} = ∏_{i=0}^{k-2} (β+n+k+i)
        for (K i = static_cast<K>(0); i < order - static_cast<K>(1); ++i){
            up_pochamer   *= (beta_in_use + static_cast<T>(n + j     + i));
            down_pochamer *= (beta_in_use + static_cast<T>(n + order + i));
        }


        rest *= (up_pochamer / down_pochamer);  // Multiply by Pochhammer ratio
        rest *= remainder->operator()(
            n + j,        // Multiply by remainder term 1/R_{n+j}
            n + j,
            data.an,
            (remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<T>(1))
        );

        // Accumulate numerator and denominator
        numerator   += rest * data.Sn.at( + j);
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

    using std::isfinite;

    // For theory, see: Sidi (2003, arXiv:math/0306302), Eqs. (8.3)-(8.5)
    // Recursive implementation using the E-algorithm scheme
    std::vector<T>   Num(
        order + static_cast<K>(1), 
        static_cast<T>(0.0)
    );
    std::vector<T> Denom(
        order + static_cast<K>(1), 
        static_cast<T>(0.0)
    );

    // Initialize base values: E_0^{(n)} = S_n, g_0^{(n)} = 1/R_n
    for (K i = static_cast<K>(0); i <= order; ++i){

        Denom[i] = remainder->operator()(
            n + i,
            n + i,
            data.an,
            (remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<T>(1))
        );

        Num[i] = data.Sn.at( + i) * Denom[i];

    }

    T scale1, scale2;

    // Recursive computation using the E-algorithm recurrence
    for (K i = static_cast<K>(1); i <= order; ++i)
        for(K j = static_cast<K>(0); j <= order - i; ++j){

            // i ~ k from formula
            // j ~ n from formula

            // For theory, see: Sidi (2003), Eqs. (8.4)-(8.5)
            // Compute scaling factors based on Pochhammer symbol ratios

            scale1 = beta_in_use + static_cast<T>(n + i + j);
            scale1*= (scale1 + static_cast<T>(1));

            scale2 = scale1 + static_cast<T>(n + i);
            scale2*= (scale2 + static_cast<T>(1));

            // Apply recurrence: E_k^{(n)} = E_{k-1}^{(n)} - g_{k-1,k}^{(n)} * ΔE_{k-1}^{(n)} / Δg_{k-1,k}^{(n)}
            Denom[j] = fma(-scale1,Denom[j]/scale2,Denom[j+static_cast<K>(1)]);
              Num[j] = fma(-scale1,  Num[j]/scale2,  Num[j+static_cast<K>(1)]);
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
        throw std::out_of_range("The Sn or an smaller then required for S_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;

    const T result = (use_recurrent_formula ? calc_result_rec(n,order, data) : calc_result(n, order, data));

    if(!isfinite(result)){
        throw std::overflow_error("division by zero");
    }

    return result;
}

#ifdef INC_FPRECISION

template<UnsignedIntLike K>
class levin_sidi_s_algorithm<float_precision, K> final : public series_acceleration<float_precision, K> {
protected:

    float_precision beta_in_use;                                                 ///< Positive real parameter (β > 0). Default value is 1.0.
    std::unique_ptr<const transform_base<float_precision, K>> remainder;  ///< Pointer to remainder transformation object
    bool use_recurrent_formula = false;                            ///< Flag to use recurrence formulas (true) or direct formulas (false)
    remainder_type remainder_type_in_use = remainder_type::u_type;     ///< Type of Levin transformation variant (u, t, v, t~, v~)

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
    inline float_precision calc_result(
        const K n, 
        const K order, 
        const series_result<float_precision>& data
    ) const;

    /**
	* @brief Function to calculate S-tranformation using recurrence formula.
	* @param n The partial sum number (S_n) from which the calculations will be done
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/

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
    inline float_precision calc_result_rec(
        const K n, 
        const K order, 
        const series_result<float_precision>& data
    ) const;

public:

    /**
     * @brief Parameterized constructor to initialize the Levin-Sidi S-transformation.
     *
     * @param series The series class object to be accelerated
     *        Must be a valid object implementing the required series interface
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
        const float_precision& beta_to_use = static_cast<float_precision>(1)
    ) : series_acceleration<float_precision, K>(), use_recurrent_formula(use_recurrent_formula){
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
    float_precision operator()(
        const K n, 
        const K order, 
        const series_result<float_precision>& data
    ) const override;

    void update_beta(const float_precision& new_beta){ 
        beta_in_use = (new_beta > static_cast<float_precision>(0) ?  new_beta : static_cast<float_precision>(1)); }

    void update_type(const remainder_type remainder_type_to_use){

		remainder_type_in_use = remainder_type_to_use;

		switch(remainder_type_to_use){
        	case remainder_type::u_type 	: { remainder.reset(new u_transform<float_precision, K>()	   ); break; }
        	case remainder_type::t_type 	: { remainder.reset(new t_transform<float_precision, K>()	   ); break; }
        	case remainder_type::v_type 	: { remainder.reset(new v_transform<float_precision, K>()	   ); break; }
        	case remainder_type::t_wave_type: { remainder.reset(new t_wave_transform<float_precision, K>()); break; }
        	case remainder_type::v_wave_type: { remainder.reset(new v_wave_transform<float_precision, K>()); break; }
        	default:{
				remainder_type_in_use = remainder_type::u_type;
        	    remainder.reset(new u_transform<float_precision, K>()); // Default to u-variant
			}
		}
	}

	std::string get_name() {

		using std::to_string;

		series_acceleration<float_precision, K>::acceleration_name = (use_recurrent_formula ? "recurrent " : "");
		series_acceleration<float_precision, K>::acceleration_name += "levin s algorithm ";
		switch(remainder_type_in_use){
			case remainder_type::u_type 	: { series_acceleration<float_precision, K>::acceleration_name += "with u-variant "; 		break; }
			case remainder_type::t_type 	: { series_acceleration<float_precision, K>::acceleration_name += "with t-variant "; 		break; }
			case remainder_type::v_type 	: { series_acceleration<float_precision, K>::acceleration_name += "with v-variant "; 		break; }
			case remainder_type::t_wave_type: { series_acceleration<float_precision, K>::acceleration_name += "with t-wave-variant "; break; }
			case remainder_type::v_wave_type: { series_acceleration<float_precision, K>::acceleration_name += "with v-wave-variant "; break; }
		}
		series_acceleration<float_precision, K>::acceleration_name += "and beta = " + to_string(beta_in_use);

        return series_acceleration<float_precision, K>::acceleration_name;
	}

};

template<UnsignedIntLike K>
inline float_precision levin_sidi_s_algorithm<float_precision, K>::calc_result(
    const K n, 
    const K order, 
    const series_result<float_precision>& data
) const {

    using std::isfinite;

    const size_t precision = std::max(data.Sn[0].precision(), data.an[0].precision());

    float_precision numerator   = float_precision(0.0, precision);
    float_precision denominator = float_precision(0.0, precision);
    float_precision rest;
    float_precision up_pochamer, down_pochamer;

    // For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (8.2)
    // S_{k,n} = [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} S_{n+j}/R_{n+j}] /
    //           [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} 1/R_{n+j}]
    for (K j = static_cast<K>(0); j <= order; ++j){

        // Compute (-1)^j * C(k,j)
        rest  = float_precision(1.0, precision);
        rest *= minus_one_raised_to_power_n<float_precision,K>(j);
        rest *= float_precision(binomial_coefficient<K>(order, j));

        // Compute Pochhammer symbols: (β+n+j)_{k-1} and (β+n+k)_{k-1}
        up_pochamer = down_pochamer = float_precision(1.0, precision);
        //up_pochamer   (beta + n + j)_(order - 1)     = (beta + n + j)(beta + n + j + 1)...(beta + n + j + order - 2)
        //down_pochamer (beta + n + order)_(order - 1) = (beta + n + order)(beta + n + order + 1)...(beta + n + order + oreder - 2)

        // (β+n+j)_{k-1} = ∏_{i=0}^{k-2} (β+n+j+i)
        // (β+n+k)_{k-1} = ∏_{i=0}^{k-2} (β+n+k+i)
        for (K i = static_cast<K>(0); i < order - static_cast<K>(1); ++i){
            up_pochamer   *= (beta_in_use + static_cast<float_precision>(n + j     + i));
            down_pochamer *= (beta_in_use + static_cast<float_precision>(n + order + i));
        }


        rest *= (up_pochamer / down_pochamer);  // Multiply by Pochhammer ratio
        rest *= remainder->operator()(
            n + j,        // Multiply by remainder term 1/R_{n+j}
            n + j,
            data.an,
            (remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<float_precision>(1))
        );

        // Accumulate numerator and denominator
        numerator   += rest * data.Sn.at(n + j);
        denominator += rest;
    }

    numerator /= denominator;

    return numerator;
}

template<UnsignedIntLike K>
inline float_precision levin_sidi_s_algorithm<float_precision, K>::calc_result_rec(
    const K n, 
    const K order, 
    const series_result<float_precision>& data
) const {

    using std::isfinite;

    const size_t precision = std::max(data.Sn[0].precision(), data.an[0].precision());

    // For theory, see: Sidi (2003, arXiv:math/0306302), Eqs. (8.3)-(8.5)
    // Recursive implementation using the E-algorithm scheme
    std::vector<float_precision>   Num(
        order + static_cast<K>(1), 
        float_precision(0.0, precision)
    );
    std::vector<float_precision> Denom(
        order + static_cast<K>(1), 
        float_precision(0.0, precision)
    );

    // Initialize base values: E_0^{(n)} = S_n, g_0^{(n)} = 1/R_n
    for (K i = static_cast<K>(0); i <= order; ++i){

        Denom[i] = remainder->operator()(
            n + i,
            n + i,
            data.an,
            (remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<float_precision>(1))
        );

        Num[i] = data.Sn.at(n + i) * Denom[i];

    }

    float_precision scale1 = float_precision(0.0, precision);
    float_precision scale2 = float_precision(0.0, precision);

    // Recursive computation using the E-algorithm recurrence
    for (K i = static_cast<K>(1); i <= order; ++i)
        for(K j = static_cast<K>(0); j <= order - i; ++j){

            // i ~ k from formula
            // j ~ n from formula

            // For theory, see: Sidi (2003), Eqs. (8.4)-(8.5)
            // Compute scaling factors based on Pochhammer symbol ratios

            scale1 = beta_in_use + static_cast<float_precision>(n + i + j);
            scale1*= (scale1 + static_cast<float_precision>(1));

            scale2 = scale1 + static_cast<float_precision>(n + i);
            scale2*= (scale2 + static_cast<float_precision>(1));

            // Apply recurrence: E_k^{(n)} = E_{k-1}^{(n)} - g_{k-1,k}^{(n)} * ΔE_{k-1}^{(n)} / Δg_{k-1,k}^{(n)}
            Denom[j] = fma(-scale1,Denom[j]/scale2,Denom[j+static_cast<K>(1)]);
              Num[j] = fma(-scale1,  Num[j]/scale2,  Num[j+static_cast<K>(1)]);
        }

    Num[0] /= Denom[0];

    return Num[0];
}

template<UnsignedIntLike K>
float_precision levin_sidi_s_algorithm<float_precision, K>::operator()(
    const K n, 
    const K order, 
    const series_result<float_precision>& data
) const{

    const K required_size = n + order + static_cast<K>(1) + static_cast<K>(
		remainder_type_in_use == remainder_type::t_wave_type ||
		remainder_type_in_use == remainder_type::v_type ||
		remainder_type_in_use == remainder_type::v_wave_type
	);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for S_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;

    const float_precision result = (use_recurrent_formula ? calc_result_rec(n,order, data) : calc_result(n, order, data));
    if(!isfinite(result)){
        throw std::overflow_error("division by zero");
    }

    return result;
}

#ifdef INC_COMPLEXPRECISION

template<std::floating_point T, UnsignedIntLike K>
class levin_sidi_s_algorithm<complex_precision<T>, K> final : public series_acceleration<complex_precision<T>, K> {
protected:

    T beta_in_use;                                                 ///< Positive real parameter (β > 0). Default value is 1.0.
    std::unique_ptr<const transform_base<complex_precision<T>, K>> remainder;  ///< Pointer to remainder transformation object
    bool use_recurrent_formula = false;                            ///< Flag to use recurrence formulas (true) or direct formulas (false)
    remainder_type remainder_type_in_use = remainder_type::u_type;     ///< Type of Levin transformation variant (u, t, v, t~, v~)

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
    inline complex_precision<T> calc_result(
        const K n, 
        const K order, 
        const series_result<complex_precision<T>>& data
    ) const;

    /**
	* @brief Function to calculate S-tranformation using recurrence formula.
	* @param n The partial sum number (S_n) from which the calculations will be done
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/

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
    inline complex_precision<T> calc_result_rec(
        const K n, 
        const K order, 
        const series_result<complex_precision<T>>& data
    ) const;

public:

    /**
     * @brief Parameterized constructor to initialize the Levin-Sidi S-transformation.
     *
     * @param series The series class object to be accelerated
     *        Must be a valid object implementing the required series interface
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
        const T& beta_to_use = static_cast<T>(1)
    ) : series_acceleration<complex_precision<T>, K>(), use_recurrent_formula(use_recurrent_formula) {
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
    complex_precision<T> operator()(
        const K n, 
        const K order, 
        const series_result<complex_precision<T>>& data
    ) const override;

    void update_beta(const T& new_beta){ beta_in_use = (new_beta > static_cast<T>(0) ?  new_beta : static_cast<T>(1)); }

    void update_type(const remainder_type remainder_type_to_use){

		remainder_type_in_use = remainder_type_to_use;

		switch(remainder_type_to_use){
        	case remainder_type::u_type 	: { remainder.reset(new u_transform<complex_precision<T>, K>()	   ); break; }
        	case remainder_type::t_type 	: { remainder.reset(new t_transform<complex_precision<T>, K>()	   ); break; }
        	case remainder_type::v_type 	: { remainder.reset(new v_transform<complex_precision<T>, K>()	   ); break; }
        	case remainder_type::t_wave_type: { remainder.reset(new t_wave_transform<complex_precision<T>, K>()); break; }
        	case remainder_type::v_wave_type: { remainder.reset(new v_wave_transform<complex_precision<T>, K>()); break; }
        	default:{
				remainder_type_in_use = remainder_type::u_type;
        	    remainder.reset(new u_transform<complex_precision<T>, K>()); // Default to u-variant
			}
		}
	}

	std::string get_name() {

		using std::to_string;

		series_acceleration<complex_precision<T>, K>::acceleration_name = (use_recurrent_formula ? "recurrent " : "");
		series_acceleration<complex_precision<T>, K>::acceleration_name += "levin sidi s algorithm ";
		switch(remainder_type_in_use){
			case remainder_type::u_type 	: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with u-variant "; 		break; }
			case remainder_type::t_type 	: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with t-variant "; 		break; }
			case remainder_type::v_type 	: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with v-variant "; 		break; }
			case remainder_type::t_wave_type: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with t-wave-variant "; break; }
			case remainder_type::v_wave_type: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with v-wave-variant "; break; }
		}
		series_acceleration<complex_precision<T>, K>::acceleration_name += "and beta = " + to_string(beta_in_use);

        return series_acceleration<complex_precision<T>, K>::acceleration_name;
	}

};

template<std::floating_point T, UnsignedIntLike K>
inline complex_precision<T> levin_sidi_s_algorithm<complex_precision<T>, K>::calc_result(
    const K n, 
    const K order, 
    const series_result<complex_precision<T>>& data
) const {

    using std::isfinite;

    complex_precision<T> numerator   = complex_precision<T>(0.0);
    complex_precision<T> denominator = complex_precision<T>(0.0);
    complex_precision<T> rest;
    complex_precision<T> up_pochamer, down_pochamer;

    // For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (8.2)
    // S_{k,n} = [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} S_{n+j}/R_{n+j}] /
    //           [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} 1/R_{n+j}]
    for (K j = static_cast<K>(0); j <= order; ++j){

        // Compute (-1)^j * C(k,j)
        rest  = complex_precision<T>(1.0);
        rest *= minus_one_raised_to_power_n<complex_precision<T>,K>(j);
        rest *= complex_precision<T>(binomial_coefficient<K>(order, j));

        // Compute Pochhammer symbols: (β+n+j)_{k-1} and (β+n+k)_{k-1}
        up_pochamer = down_pochamer = complex_precision<T>(1.0);
        //up_pochamer   (beta + n + j)_(order - 1)     = (beta + n + j)(beta + n + j + 1)...(beta + n + j + order - 2)
        //down_pochamer (beta + n + order)_(order - 1) = (beta + n + order)(beta + n + order + 1)...(beta + n + order + oreder - 2)

        // (β+n+j)_{k-1} = ∏_{i=0}^{k-2} (β+n+j+i)
        // (β+n+k)_{k-1} = ∏_{i=0}^{k-2} (β+n+k+i)
        for (K i = static_cast<K>(0); i < order - static_cast<K>(1); ++i){
            up_pochamer   *= complex_precision<T>(beta_in_use + static_cast<T>(n + j     + i));
            down_pochamer *= complex_precision<T>(beta_in_use + static_cast<T>(n + order + i));
        }


        rest *= (up_pochamer / down_pochamer);  // Multiply by Pochhammer ratio
        rest *= remainder->operator()(
            n + j,        // Multiply by remainder term 1/R_{n+j}
            n + j,
            data.an,
            complex_precision<T>(remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<T>(1))
        );

        // Accumulate numerator and denominator
        numerator   += rest * data.Sn.at(n + j);
        denominator += rest;
    }

    numerator /= denominator;

    return numerator;
}

template<std::floating_point T, UnsignedIntLike K>
inline complex_precision<T> levin_sidi_s_algorithm<complex_precision<T>, K>::calc_result_rec(
    const K n, 
    const K order, 
    const series_result<complex_precision<T>>& data
) const {

    using std::isfinite;

    // For theory, see: Sidi (2003, arXiv:math/0306302), Eqs. (8.3)-(8.5)
    // Recursive implementation using the E-algorithm scheme
    std::vector<complex_precision<T>>   Num(
        order + static_cast<K>(1), 
        complex_precision<T>(0.0)
    );
    std::vector<complex_precision<T>> Denom(
        order + static_cast<K>(1), 
        complex_precision<T>(0.0)
    );

    // Initialize base values: E_0^{(n)} = S_n, g_0^{(n)} = 1/R_n
    for (K i = static_cast<K>(0); i <= order; ++i){

        Denom[i] = remainder->operator()(
            n + i,
            n + i,
            data.an,
            complex_precision<T>(remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<T>(1))
        );

        Num[i] = data.Sn.at(n + i) * Denom[i];

    }

    T scale1, scale2;

    // Recursive computation using the E-algorithm recurrence
    for (K i = static_cast<K>(1); i <= order; ++i)
        for(K j = static_cast<K>(0); j <= order - i; ++j){

            // i ~ k from formula
            // j ~ n from formula

            // For theory, see: Sidi (2003), Eqs. (8.4)-(8.5)
            // Compute scaling factors based on Pochhammer symbol ratios

            scale1 = beta_in_use + static_cast<T>(n + i + j);
            scale1*= (scale1 + static_cast<T>(1));

            scale2 = scale1 + static_cast<T>(n + i);
            scale2*= (scale2 + static_cast<T>(1));

            // Apply recurrence: E_k^{(n)} = E_{k-1}^{(n)} - g_{k-1,k}^{(n)} * ΔE_{k-1}^{(n)} / Δg_{k-1,k}^{(n)}
            Denom[j] = fma(
                complex_precision<T>(-scale1),
                Denom[j]/complex_precision<T>(scale2),
                Denom[j+static_cast<K>(1)]
            );
            Num[j] = fma(
                complex_precision<T>(-scale1),  
                Num[j]/complex_precision<T>(scale2),  
                Num[j+static_cast<K>(1)]
            );
        }

    Num[0] /= Denom[0];

    return Num[0];
}

template<std::floating_point T, UnsignedIntLike K>
complex_precision<T> levin_sidi_s_algorithm<complex_precision<T>, K>::operator()(
    const K n, 
    const K order, 
    const series_result<complex_precision<T>>& data
) const{

    const K required_size = n + order + static_cast<K>(1) + static_cast<K>(
		remainder_type_in_use == remainder_type::t_wave_type ||
		remainder_type_in_use == remainder_type::v_type ||
		remainder_type_in_use == remainder_type::v_wave_type
	);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for S_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;

    const complex_precision<T> result = (use_recurrent_formula ? calc_result_rec(n,order, data) : calc_result(n, order, data));

    if(!isfinite(result.real()) || !isfinite(result.imag())){
        throw std::overflow_error("division by zero");
    }

    return result;
}

template<UnsignedIntLike K>
class levin_sidi_s_algorithm<complex_precision<float_precision>, K> final : public series_acceleration<complex_precision<float_precision>, K> {
protected:

    float_precision beta_in_use;                                                 ///< Positive real parameter (β > 0). Default value is 1.0.
    std::unique_ptr<const transform_base<complex_precision<float_precision>, K>> remainder;  ///< Pointer to remainder transformation object
    bool use_recurrent_formula = false;                            ///< Flag to use recurrence formulas (true) or direct formulas (false)
    remainder_type remainder_type_in_use = remainder_type::u_type;     ///< Type of Levin transformation variant (u, t, v, t~, v~)

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
    inline complex_precision<float_precision> calc_result(
        const K n, 
        const K order, 
        const series_result<complex_precision<float_precision>>& data
    ) const;

    /**
	* @brief Function to calculate S-tranformation using recurrence formula.
	* @param n The partial sum number (S_n) from which the calculations will be done
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/

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
    inline complex_precision<float_precision> calc_result_rec(
        const K n, 
        const K order, 
        const series_result<complex_precision<float_precision>>& data
    ) const;

public:

    /**
     * @brief Parameterized constructor to initialize the Levin-Sidi S-transformation.
     *
     * @param series The series class object to be accelerated
     *        Must be a valid object implementing the required series interface
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
        remainder_type remainder_type_to_use = remainder_type::u_type,
        bool use_recurrent_formula = false,
        const float_precision& beta_to_use = static_cast<float_precision>(1)
    ) : series_acceleration<complex_precision<float_precision>, K>(), use_recurrent_formula(use_recurrent_formula){
        // parameter is "beta" parameter
        // beta must be nonzero positive real number
        // beta = 1 is default
        // check parameter else default
        update_beta(beta_to_use);
        update_type(remainder_type_to_use);
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
    complex_precision<float_precision> operator()(
        const K n, 
        const K order, 
        const series_result<complex_precision<float_precision>>& data
    ) const override;

    void update_beta(const float_precision& new_beta){ 
        beta_in_use = (new_beta > static_cast<float_precision>(0) ?  new_beta : static_cast<float_precision>(1)); 
    }

    void update_type(const remainder_type remainder_type_to_use){

		remainder_type_in_use = remainder_type_to_use;

        using ComplexFloatPrec = complex_precision<float_precision>;

		switch(remainder_type_to_use){
        	case remainder_type::u_type 	: { remainder.reset(new u_transform<ComplexFloatPrec, K>()	   ); break; }
        	case remainder_type::t_type 	: { remainder.reset(new t_transform<ComplexFloatPrec, K>()	   ); break; }
        	case remainder_type::v_type 	: { remainder.reset(new v_transform<ComplexFloatPrec, K>()	   ); break; }
        	case remainder_type::t_wave_type: { remainder.reset(new t_wave_transform<ComplexFloatPrec, K>()); break; }
        	case remainder_type::v_wave_type: { remainder.reset(new v_wave_transform<ComplexFloatPrec, K>()); break; }
        	default:{
				remainder_type_in_use = remainder_type::u_type;
        	    remainder.reset(new u_transform<ComplexFloatPrec, K>()); // Default to u-variant
			}
		}
	}

	std::string get_name() {

		using std::to_string;

        using ComplexFloatPrec = complex_precision<float_precision>;

		series_acceleration<ComplexFloatPrec, K>::acceleration_name = (use_recurrent_formula ? "recurrent " : "");
		series_acceleration<ComplexFloatPrec, K>::acceleration_name += "levin sidi s algorithm ";
		switch(remainder_type_in_use){
			case remainder_type::u_type 	: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with u-variant "; 		break; }
			case remainder_type::t_type 	: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with t-variant "; 		break; }
			case remainder_type::v_type 	: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with v-variant "; 		break; }
			case remainder_type::t_wave_type: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with t-wave-variant "; break; }
			case remainder_type::v_wave_type: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with v-wave-variant "; break; }
		}
		series_acceleration<ComplexFloatPrec, K>::acceleration_name += "and beta = " + to_string(beta_in_use);

        return series_acceleration<ComplexFloatPrec, K>::acceleration_name;
	}

};

template<UnsignedIntLike K>
inline complex_precision<float_precision> levin_sidi_s_algorithm<complex_precision<float_precision>, K>::calc_result(
    const K n, 
    const K order, 
    const series_result<complex_precision<float_precision>>& data
) const {

    using std::isfinite;

    const size_t precision = std::max(
        std::max(data.Sn[0].real().precision(),data.Sn[0].imag().precision()),
        std::max(data.an[0].real().precision(),data.an[0].imag().precision())
    );

    complex_precision<float_precision> numerator   = complex_precision<float_precision>(
        float_precision(0.0, precision),
        float_precision(0.0, precision)
    );
    complex_precision<float_precision> denominator = complex_precision<float_precision>(
        float_precision(0.0, precision),
        float_precision(0.0, precision)
    );
    complex_precision<float_precision> rest;
    complex_precision<float_precision> up_pochamer, down_pochamer;

    // For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (8.2)
    // S_{k,n} = [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} S_{n+j}/R_{n+j}] /
    //           [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} 1/R_{n+j}]
    for (K j = static_cast<K>(0); j <= order; ++j){

        // Compute (-1)^j * C(k,j)
        rest  = complex_precision<float_precision>(
            float_precision(1.0, precision),
            float_precision(0.0, precision)
        );
        rest *= minus_one_raised_to_power_n<complex_precision<float_precision>,K>(j);
        rest *= complex_precision<float_precision>(binomial_coefficient<K>(order, j));

        // Compute Pochhammer symbols: (β+n+j)_{k-1} and (β+n+k)_{k-1}
        up_pochamer = down_pochamer = float_precision(1.0, precision);
        //up_pochamer   (beta + n + j)_(order - 1)     = (beta + n + j)(beta + n + j + 1)...(beta + n + j + order - 2)
        //down_pochamer (beta + n + order)_(order - 1) = (beta + n + order)(beta + n + order + 1)...(beta + n + order + oreder - 2)

        // (β+n+j)_{k-1} = ∏_{i=0}^{k-2} (β+n+j+i)
        // (β+n+k)_{k-1} = ∏_{i=0}^{k-2} (β+n+k+i)
        for (K i = static_cast<K>(0); i < order - static_cast<K>(1); ++i){
            up_pochamer   *= complex_precision<float_precision>(beta_in_use + static_cast<float_precision>(n + j     + i));
            down_pochamer *= complex_precision<float_precision>(beta_in_use + static_cast<float_precision>(n + order + i));
        }


        rest *= (up_pochamer / down_pochamer);  // Multiply by Pochhammer ratio
        rest *= remainder->operator()(
            n + j,        // Multiply by remainder term 1/R_{n+j}
            n + j,
            data.an,
            complex_precision<float_precision>(remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<float_precision>(1))
        );

        // Accumulate numerator and denominator
        numerator   += rest * data.Sn.at(n + j);
        denominator += rest;
    }

    numerator /= denominator;

    return numerator;
}

template<UnsignedIntLike K>
inline complex_precision<float_precision> levin_sidi_s_algorithm<complex_precision<float_precision>, K>::calc_result_rec(
    const K n, 
    const K order, 
    const series_result<complex_precision<float_precision>>& data
) const {

    using std::isfinite;

    const size_t precision = std::max(
        std::max(data.Sn[0].real().precision(),data.Sn[0].imag().precision()),
        std::max(data.an[0].real().precision(),data.an[0].imag().precision())
    );

    // For theory, see: Sidi (2003, arXiv:math/0306302), Eqs. (8.3)-(8.5)
    // Recursive implementation using the E-algorithm scheme
    std::vector<complex_precision<float_precision>>   Num(
        order + static_cast<K>(1), 
        complex_precision<float_precision>(
            float_precision(0.0, precision),
            float_precision(0.0, precision)
        )
    );
    std::vector<complex_precision<float_precision>> Denom(
        order + static_cast<K>(1), 
        complex_precision<float_precision>(
            float_precision(0.0, precision),
            float_precision(0.0, precision)
        )
    );

    // Initialize base values: E_0^{(n)} = S_n, g_0^{(n)} = 1/R_n
    for (K i = static_cast<K>(0); i <= order; ++i){

        Denom[i] = remainder->operator()(
            n + i,
            n + i,
            data.an,
            complex_precision<float_precision>(remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<float_precision>(1))
        );

        Num[i] = data.Sn.at(n + i) * Denom[i];

    }

    float_precision scale1 = float_precision(0.0, precision);
    float_precision scale2 = float_precision(0.0, precision);

    // Recursive computation using the E-algorithm recurrence
    for (K i = static_cast<K>(1); i <= order; ++i)
        for(K j = static_cast<K>(0); j <= order - i; ++j){

            // i ~ k from formula
            // j ~ n from formula

            // For theory, see: Sidi (2003), Eqs. (8.4)-(8.5)
            // Compute scaling factors based on Pochhammer symbol ratios
            scale1 = beta_in_use + float_precision(n + i + j);
            scale1*= (scale1 + float_precision(1));

            scale2 = scale1 + float_precision(n + i);
            scale2*= (scale2 + float_precision(1));

            // Apply recurrence: E_k^{(n)} = E_{k-1}^{(n)} - g_{k-1,k}^{(n)} * ΔE_{k-1}^{(n)} / Δg_{k-1,k}^{(n)}
            Denom[j] = fma(
                complex_precision<float_precision>(-scale1),
                Denom[j]/complex_precision<float_precision>(scale2),
                Denom[j+static_cast<K>(1)]
            );
            Num[j] = fma(
                complex_precision<float_precision>(-scale1),  
                Num[j]/complex_precision<float_precision>(scale2),  
                Num[j+static_cast<K>(1)]
            );
        }

    Num[0] /= Denom[0];

    return Num[0];
}

template<UnsignedIntLike K>
complex_precision<float_precision> levin_sidi_s_algorithm<complex_precision<float_precision>, K>::operator()(
    const K n, 
    const K order, 
    const series_result<complex_precision<float_precision>>& data
) const{

    const K required_size = n + order + static_cast<K>(1) + static_cast<K>(
		remainder_type_in_use == remainder_type::t_wave_type ||
		remainder_type_in_use == remainder_type::v_type ||
		remainder_type_in_use == remainder_type::v_wave_type
	);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for S_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;

    const complex_precision<float_precision> result = (use_recurrent_formula ? calc_result_rec(n,order, data) : calc_result(n, order, data));

    if(!isfinite(result)){
        throw std::overflow_error("division by zero");
    }

    return result;
}
#endif
#endif