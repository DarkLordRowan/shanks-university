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
#include <memory>                    // Include for unique ptr

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
  * @tparam T Floating-point type for series elements (must satisfy std::floating_point)
  *           Represents numerical precision (float, double, long double)
  * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
  *           Used for counting and indexing operations
  * @tparam series_templ Type of series object to accelerate. Must provide:
  *           - T operator()(K n) const: returns the n-th series term a_n
  *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
  *           - T minus_one_raised_to_power_n(K j) const: returns (-1)^j
  *           - T binomial_coefficient(T n, K k) const: returns binomial coefficient C(n, k)
  */
template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class levin_sidi_s_algorithm final : public series_acceleration<T, K, series_templ> {
protected:

    T beta;                                                 ///< Positive real parameter (β > 0). Default value is 1.0.
    std::unique_ptr<const transform_base<T, K>> remainder;  ///< Pointer to remainder transformation object
    bool useRecFormulas = false;                            ///< Flag to use recurrence formulas (true) or direct formulas (false)
    remainder_type variant = remainder_type::u_variant;     ///< Type of Levin transformation variant (u, t, v, t~, v~)

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
    inline T calc_result(K n, K order) const;

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
    inline T calc_result_rec(K n, K order) const;

public:

    /**
     * @brief Parameterized constructor to initialize the Levin-Sidi S-transformation.
     *
     * @param series The series class object to be accelerated
     *        Must be a valid object implementing the required series interface
     * @param variant Type of remainder transformation to use
     *        Valid values: u_variant, t_variant, v_variant, t_wave_variant, v_wave_variant
     *        Determines the remainder estimate R_n used in the transformation
     * @param useRecFormulas Flag to use recurrence formulas instead of direct summation
     *        true: use recursive implementation, false: use direct summation
     * @param parameter Positive real parameter β (must be > 0)
     *        Default value: 1.0. Affects the Pochhammer symbol terms in the transformation.
     *        For theory, see: Sidi (2003, arXiv:math/0306302), p. 39
     */
    explicit levin_sidi_s_algorithm(
        const series_templ& series,
        remainder_type variant = remainder_type::u_variant,
        bool useRecFormulas = false,  
        T parameter = static_cast<T>(1)
    );


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
    T operator()(K n, K order) const override;

};

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T levin_sidi_s_algorithm<T, K,series_templ>::calc_result(K n, K order) const{

    using std::isfinite;

    T numerator = static_cast<T>(0), denominator = static_cast<T>(0);
    T rest;
    T up_pochamer, down_pochamer; 

    // For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (8.2)
    // S_{k,n} = [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} S_{n+j}/R_{n+j}] /
    //           [∑_{j=0}^k (-1)^j C(k,j) (β+n+j)_{k-1}/(β+n+k)_{k-1} 1/R_{n+j}]
    for (K j = static_cast<K>(0); j <= order; ++j){

        // Compute (-1)^j * C(k,j)
        rest  = this->series->minus_one_raised_to_power_n(j);
        rest *= this->series->binomial_coefficient(static_cast<T>(order), j);

        // Compute Pochhammer symbols: (β+n+j)_{k-1} and (β+n+k)_{k-1}
        up_pochamer = down_pochamer = static_cast<T>(1);
        //up_pochamer   (beta + n + j)_(order - 1)     = (beta + n + j)(beta + n + j + 1)...(beta + n + j + order - 2)
        //down_pochamer (beta + n + order)_(order - 1) = (beta + n + order)(beta + n + order + 1)...(beta + n + order + oreder - 2)

        // (β+n+j)_{k-1} = ∏_{i=0}^{k-2} (β+n+j+i)
        // (β+n+k)_{k-1} = ∏_{i=0}^{k-2} (β+n+k+i)
        for (K i = static_cast<K>(0); i < order - static_cast<K>(1); ++i){
            up_pochamer   *= (beta + static_cast<T>(n + j     + i));
            down_pochamer *= (beta + static_cast<T>(n + order + i));
        }

        
        rest *= (up_pochamer / down_pochamer);  // Multiply by Pochhammer ratio
        rest *= remainder->operator()(n,        // Multiply by remainder term 1/R_{n+j}
            j, 
            this->series,
             (variant == remainder_type::u_variant ? beta : static_cast<T>(1))
            );

        // Accumulate numerator and denominator
        numerator   += rest * this->series->S_n(n + j);
        denominator += rest;
    }

    numerator /= denominator;
    
    if (!isfinite(numerator)) throw std::overflow_error("division by zero");
    return numerator;
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T levin_sidi_s_algorithm<T, K,series_templ>::calc_result_rec(K n, K order) const{

    using std::isfinite;

    // For theory, see: Sidi (2003, arXiv:math/0306302), Eqs. (8.3)-(8.5)
    // Recursive implementation using the E-algorithm scheme
    std::vector<T>   Num(order + static_cast<K>(1), static_cast<T>(0));
    std::vector<T> Denom(order + static_cast<K>(1), static_cast<T>(0));

    // Initialize base values: E_0^{(n)} = S_n, g_0^{(n)} = 1/R_n
    for (K i = static_cast<K>(0); i <= order; ++i){

        Denom[i] = remainder->operator()(
            n, 
            i, 
            this->series,
            (variant == remainder_type::u_variant ? beta : static_cast<T>(1))
        );

        Num[i] = this->series->S_n(n + i); Num[i] *= Denom[i];
    }
    
    T scale1, scale2;

    // Recursive computation using the E-algorithm recurrence
    for (K i = static_cast<K>(1); i <= order; ++i)
        for(K j = static_cast<K>(0); j <= order - i; ++j){

            // i ~ k from formula
            // j ~ n from formula

            // For theory, see: Sidi (2003), Eqs. (8.4)-(8.5)
            // Compute scaling factors based on Pochhammer symbol ratios
            scale1 = beta + static_cast<T>(i + j);
            scale1*= (scale1 - static_cast<T>(1));

            scale2 = scale1 + static_cast<T>(i);
            scale2*= (scale2 - static_cast<T>(1));

            // Apply recurrence: E_k^{(n)} = E_{k-1}^{(n)} - g_{k-1,k}^{(n)} * ΔE_{k-1}^{(n)} / Δg_{k-1,k}^{(n)}
            Denom[j] = fma(-scale1,Denom[j]/scale2,Denom[j+static_cast<K>(1)]);
              Num[j] = fma(-scale1,  Num[j]/scale2,  Num[j+static_cast<K>(1)]);
        }
    
    Num[0] /= Denom[0];
    
    if (!isfinite(Num[0])) throw std::overflow_error("division by zero");
    return Num[0];
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
levin_sidi_s_algorithm<T, K, series_templ>::levin_sidi_s_algorithm(
    const series_templ& series, 
    remainder_type variant, 
    bool useRecFormulas,  
    T parameter
) : 
    series_acceleration<T, K, series_templ>(series),
    variant(variant),
    useRecFormulas(useRecFormulas)

{
    // parameter is "beta" parameter
    // beta must be nonzero positive real number
    // beta = 1 is default
    // check parameter else default
    if (parameter > static_cast<T>(0)) this->beta = parameter;
    else {
        this->beta = static_cast<T>(1);
        //TODO: наверное сообщение по типу usage
    }

    //TODO: тоже самое наверное
    // Initialize the appropriate remainder transformation based on variant
    switch(variant){
        case remainder_type::u_variant :
            remainder.reset(new u_transform<T, K>());
            break;
        case remainder_type::t_variant :
            remainder.reset(new t_transform<T, K>());
            break;
        case remainder_type::v_variant :
            remainder.reset(new v_transform<T, K>());
            break;
        case remainder_type::t_wave_variant:
            remainder.reset(new t_wave_transform<T, K>());
            break;
        case remainder_type::v_wave_variant:
            remainder.reset(new v_wave_transform<T, K>());
            break;
        default:
            remainder.reset(new u_transform<T, K>()); // Default to u-variant
    }
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T levin_sidi_s_algorithm<T, K, series_templ>::operator()(const K n, const K order) const{ 

    using std::isfinite;

    const T result = (useRecFormulas ? calc_result_rec(n,order) : calc_result(n, order));
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}