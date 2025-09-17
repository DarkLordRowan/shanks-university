/**
 * @file levin_algorithm.hpp
 * @brief This file contains the declaration of the Levin algorithm class.
 *
 * For theory, see:
 * Levin, D. (1973). Development of non-linear transformations for improving convergence of sequences.
 *   Intern. J. Computer Math., B3, 371-388.
 * Sidi, A. (1979). Convergence properties of some nonlinear sequence transformations.
 *   Math. Comp., 33, 315-326.
 * Sidi, A., & Levin, D. (1981). Two new classes of nonlinear transformations for accelerating
 *   the convergence of infinite integrals and series. Appl. Math. Comp., 9, 175-215.
 */

#pragma once

#include "../series_acceleration.hpp"
#include "../remainders.hpp"

#include <cmath> //Include for pow, isfinite
#include <vector>
#include <memory>


 /**
  * @brief Levin Algorithm class template implementing various Levin transformations.
  *
  * @authors Kreinin R.G., Trudolyubov N.A.
  *
  * This class implements the Levin transformation for series acceleration, which is particularly
  * effective for sequences with specific asymptotic behaviors. The algorithm comes in several
  * variants (u, t, v, t~, v~) that use different remainder estimates.
  *
  * References:
  * - Levin, D. (1973). Development of non-linear transformations for improving convergence of sequences.
  * - Sidi, A. (1979). Convergence properties of some nonlinear sequence transformations.
  * - Sidi, A., & Levin, D. (1981). Two new classes of nonlinear transformations.
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
template <Accepted T, std::unsigned_integral K, typename series_templ>
class levin_algorithm final : public series_acceleration<T, K, series_templ>
{
protected:

	T beta;													///< Parameter for u-variant transformation (β > 0). Default value is 1.0.
    std::unique_ptr<const transform_base<T, K>> remainder;	///< Pointer to remainder transformation object
    bool useRecFormulas = false;							///< Flag to use recurrence formulas (true) or direct formulas (false)
    remainder_type variant = remainder_type::u_variant;		///< Type of Levin transformation variant (u, t, v, t~, v~)

	/**
	 * @brief Computes the Levin transformation using direct summation formulas.
	 *
	 * For theory, see: Levin (1973), Eq. (2.3) and Sidi (1979), Eq. (2.1)
	 * General form: T_{k,n} = [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} S_{n+j}/R_{n+j}] /
	 *                      [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} 1/R_{n+j}]
	 *
	 * @param n Number of terms used in the transformation (starting index)
	 * @param order Order of transformation (k value)
	 * @return Accelerated sum estimate T_{k,n}
	 */
	inline T calc_result(K n, K order) const;

	/**
	 * @brief Computes the Levin transformation using recurrence formulas.
	 *
	 * For theory, see: Sidi (1979), Section 3 and Brezinski's E-algorithm implementation
	 * Recursive implementation for better numerical stability in some cases.
	 *
	 * @param n Number of terms used in the transformation (starting index)
	 * @param order Order of transformation (k value)
	 * @return Accelerated sum estimate T_{k,n}
	 */
	inline T calc_result_rec(K n, K order) const;

public:

	/**
	 * @brief Parameterized constructor to initialize the Levin Algorithm.
	 *
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 * @param variant Type of Levin transformation variant to use
	 *        Valid values: u_variant, t_variant, v_variant, t_wave_variant, v_wave_variant
	 *        Determines the remainder estimate R_n used in the transformation
	 * @param useRecFormulas Flag to use recurrence formulas instead of direct summation
	 *        true: use recursive implementation, false: use direct summation
	 * @param beta Parameter for u-variant transformation (must be > 0)
	 *        Default value: 1.0. Affects the remainder estimate in u-variant.
	 *        For theory, see: Sidi & Levin (1981), Eq. (3.4) and surrounding discussion
	 */
	explicit levin_algorithm(
		const series_templ& series,
        remainder_type variant = remainder_type::u_variant,
        bool useRecFormulas = false,  
        T beta = static_cast<T>(1)
	);

	/**
	 * @brief Implementation of Levin transformation for series acceleration.
	 *
	 * Computes the accelerated sum using the specified Levin transformation variant.
	 * The algorithm can use either direct summation or recurrence formulas based on constructor setting.
	 *
	 * For theory, see:
	 * - General framework: Levin (1973), Eq. (2.3)
	 * - Convergence properties: Sidi (1979), Theorems 3.1, 4.2
	 * - Variant-specific properties: Sidi & Levin (1981), Sections 3-4
	 * - More information, see 3.9.13 in[https://dlmf.nist.gov/3.9]
	 *
	 * @param n The number of terms to use in the transformation
	 *        Valid values: n > 0 (algorithm requires at least 1 term)
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (k value)
	 *        Valid values: order >= 0
	 *        Higher orders eliminate more terms from the asymptotic expansion but may be less stable
	 * @return The accelerated partial sum after Levin transformation
	 * @throws std::domain_error if n=0 is provided as input
	 * @throws std::overflow_error if division by zero or numerical instability occurs
	 */
	T operator()(const K n, const K order) const override;
};

template<Accepted T, std::unsigned_integral K, typename series_templ>
levin_algorithm<T, K,series_templ>::levin_algorithm(
	const series_templ& series,
    remainder_type variant,
    bool useRecFormulas,
    T beta
) :
	series_acceleration<T, K, series_templ>(series),
	useRecFormulas(useRecFormulas),
	variant(variant)
	{//TODO: нужно ли проверять бету на допустимость?

		// Validate and set beta parameter (must be positive)

		if constexpr(std::is_floating_point<T>::value || std::is_same<T, float_precision>::value){

			this->beta = (beta > static_cast<T>(0) ? beta : static_cast<T>(1));

		} else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){

			this->beta = (
				beta.real() > static_cast<float_precision>(0) && beta.imag() == static_cast<float_precision>(0) ?
				beta :
				complex_precision<float_precision>(1)
			);
			
		}	

		

	//check variant else default 'u'
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

template<Accepted T, std::unsigned_integral K, typename series_templ>
inline T levin_algorithm<T, K,series_templ>::calc_result(K n, K order) const{

	using std::pow;
	using std::isfinite;

	T numerator = static_cast<T>(0), denominator = static_cast<T>(0);
	T C_njk, S_nj, g_n, rest;

	// For theory, see: Levin (1973), Eq. (2.3)
	// T_{k,n} = [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} S_{n+j}/R_{n+j}] /
	//           [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} 1/R_{n+j}]
	for (K j = static_cast<K>(0); j <= order; ++j) {
		// Compute (-1)^j * C(k,j)
		rest  = this->series->minus_one_raised_to_power_n(j);
		rest *= this->series->binomial_coefficient(static_cast<T>(order), j);

		// Compute (n+j+1)^{k-1}/(n+k+1)^{k-1}
		C_njk  = static_cast<T>(pow(n + j     + static_cast<K>(1), order - static_cast<K>(1)));
		C_njk /= static_cast<T>(pow(n + order + static_cast<K>(1), order - static_cast<K>(1)));

		// Get partial sum S_{n+j}
		S_nj = this->series->S_n(n + j);

		// Compute 1/R_{n+j} where R_{n+j} is the remainder estimate
		g_n = static_cast<T>(1);
		g_n/= remainder->operator()(
            n + j, 
            j, 
            this->series,
            (variant == remainder_type::u_variant ? beta : static_cast<T>(1))
        );

		// Combine all terms
		rest *= C_njk;
		rest *= g_n;

		denominator += rest;
		  numerator += rest * S_nj;
	}

	numerator /= denominator;

	if (!isfinite(numerator))
		throw std::overflow_error("division by zero");

	return numerator;
}

template<Accepted T, std::unsigned_integral K, typename series_templ>
inline T levin_algorithm<T, K,series_templ>::calc_result_rec(K n, K order) const{

	using std::isfinite;

    //const T result = (*this)(n, order, beta, false) / (*this)(n, order, beta, true);

	// For theory, see: Sidi (1979), Section 3 - Recursive implementation using E-algorithm
	// Initialize arrays for recursive computation
	std::vector<T>   Num(order + static_cast<K>(1), static_cast<T>(0));
	std::vector<T> Denom(order + static_cast<K>(1), static_cast<T>(0));

	// Initialize base values: E_0^{(n)} = S_n, g_0^{(n)} = 1/R_n
	for (K i = static_cast<K>(0); i < order+static_cast<K>(1); ++i) {
		Denom[i] = remainder->operator()(
            n, 
            i, 
            this->series,
            (variant == remainder_type::u_variant ? beta : static_cast<T>(1))
        );

		Num[i] = this->series->S_n(n+i) * Denom[i];
	}

	// Recursive computation using the E-algorithm scheme
	T scale, nj;
	const T order1 = static_cast<T>(order - static_cast<K>(1));

	for (K i = static_cast<K>(1); i <= order; ++i)
		for (K j = static_cast<K>(0); j <= order - i; ++j) {

			nj = static_cast<T>(n + j);

			// For theory, see: Brezinski's E-algorithm recurrence
			// E_k^{(n)} = E_{k-1}^{(n)} - g_{k-1,k}^{(n)} * ΔE_{k-1}^{(n)} / Δg_{k-1,k}^{(n)}
			scale = -(beta + static_cast<T>(n));
			scale*= pow(static_cast<T>(1) - static_cast<T>(1) / (beta + nj + static_cast<T>(1)), order1);
			scale/=(beta + nj + static_cast<T>(1));

			Denom[j] = fma(-scale,Denom[j],Denom[j+static_cast<K>(1)]);
              Num[j] = fma(-scale,  Num[j],  Num[j+static_cast<K>(1)]);
		}

	Num[0] /= Denom[0];

	if (!isfinite(Num[0]))
		throw std::overflow_error("division by zero");
    
	return Num[0];
}

template <Accepted T, std::unsigned_integral K, typename series_templ>
T levin_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	using std::isfinite;

	if (n == static_cast<K>(0)) 
		throw std::domain_error("n = 0 in the input");

	if (order == static_cast<K>(0)) return this->series->S_n(n);

    const T result = (useRecFormulas ? calc_result_rec(n,order) : calc_result(n, order));
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}