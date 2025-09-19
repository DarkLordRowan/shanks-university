/**
 * @file levin_sidi_m_algorithm.hpp
 * @brief This file contains the definition of analogues of Levin-Sidi M-transformation
 *
 * For theory, see:
 * Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications.
 *   Cambridge University Press. (Chapter 9, pp. 285-369)
 * Sidi, A. (2003). A new class of nonlinear transformations for accelerating the convergence
 *   of infinite integrals and series. arXiv:math/0306302.
 */

#pragma once

#include "../series_acceleration.hpp"
#include "../remainders.hpp"
#include <memory> // For std::unique_ptr
#include <type_traits>

 /**
  * @brief Levin-Sidi M-transformation class template.
  *
  * @authors Yurov P.I., Bezzaborov A.A.
  *
  * This class implements the Levin-Sidi M-transformation, which is particularly effective
  * for series that belong to the b(1)/LIN/FAC classes (factorial and linear convergence).
  * The transformation is based on factorial-like terms and Pochhammer symbols.
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
template<Accepted T, std::unsigned_integral K, typename series_templ>
class levin_sidi_m_algorithm final : public series_acceleration<T, K, series_templ>
{
protected:

	const T gamma;											///< Positive real parameter such that gamma >= order - 1
	std::unique_ptr<transform_base<T, K>> remainder;	///< Pointer to remainder transformation object
	remainder_type variant;

	/**
	 * @brief Core implementation of the M-transformation using direct summation.
	 *
	 * For theory, see: Sidi (2003, arXiv:math/0306302), pp. 64-65, Eqs. (9.2)-(9.6)
	 * General form: M_{n}^{(k)} = 
	 [∑_{j=0}^n (-1)^j C(n,j) (n-j) (γ+k+2)_{n-1}/(γ+k+1)_{n} × (γ+k+1-j)_{j}/(γ+k+2-n)_{j} × 1/(j+1) × S_{k+j}/R_{k+j}] /
	 [∑_{j=0}^n (-1)^j C(n,j) (n-j) (γ+k+2)_{n-1}/(γ+k+1)_{n} × (γ+k+1-j)_{j}/(γ+k+2-n)_{j} × 1/(j+1) × 1/R_{k+j}]
	 *
	 * Note: We assume the Pochhammer symbol satisfies (-x)_n = (-1)^n*(x-n+1)_n
	 *
	 * @param n Number of terms used in the transformation
	 * @param order Order of transformation (starting index)
	 * @return Accelerated sum estimate M_{n}^{(k)}
	 */
	inline T calculate(K n, K order) const;

public:

	/**
	 * @brief Parameterized constructor to initialize the Levin-Sidi M-transformation.
	 *
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 * @param variant Type of remainder transformation to use
	 *        Valid values: u_variant, t_variant, v_variant, t_wave_variant, v_wave_variant
	 *        Determines the remainder estimate R_n used in the transformation
	 * @param gamma_ Positive real parameter such that gamma >= order - 1, see p. 64 [https://arxiv.org/pdf/math/0306302.pdf]
	 *        Default value: 10.0. Affects the factorial terms in the transformation.
	 *        For theory, see: Sidi (2003, arXiv:math/0306302), p. 64
	 */
	explicit levin_sidi_m_algorithm(
		const series_templ& series,
		remainder_type variant = remainder_type::u_variant,
		T gamma_ = static_cast<T>(10)
	);

	// Default destructor is sufficient since unique_ptr handles deletion

	/**
	 * @brief Implementation of Levin-Sidi M-transformation for series acceleration.
	 *
	 * Computes the accelerated sum using the M-transformation, which is particularly
	 * effective for series with factorial or linear convergence patterns.
	 *
	 * For theory, see:
	 * - General framework: Sidi (2003, arXiv:math/0306302), Eqs. (9.2)-(9.6)
	 * - Convergence properties: Sidi (2003, Practical Extrapolation Methods), pp. 285, 369
	 *
	 * @param n The number of terms to use in the transformation
	 *        Valid values: n > 0 (algorithm requires at least 1 term)
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (starting index k)
	 *        Valid values: order >= 0
	 *        The parameter gamma must satisfy gamma >= order - 1
	 * @return The accelerated partial sum after M-transformation
	 * @throws std::domain_error if n=0 or gamma < n-1
	 * @throws std::overflow_error if division by zero or numerical instability occurs
	 */
	T operator()(K n, K order) override;
};

template<Accepted T, std::unsigned_integral K, typename series_templ>
inline T levin_sidi_m_algorithm<T, K, series_templ>::calculate(const K n, const K order) const {

	using std::isfinite;

	//TODO разобраться с документом (pdf) n/order
    // Validate parameter constraint: gamma >= n - 1

	if constexpr (std::is_floating_point<T>::value || std::is_same<T, float_precision>::value){
		if(gamma - static_cast<T>(n - 1) < static_cast<T>(0)){
			throw std::domain_error("gamma cannot be lesser than n - 1");
		}
	}  else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
		if (gamma.real() - static_cast<float_precision>(n-1) < static_cast<float_precision>(0)){
			throw std::domain_error("gamma cannot be lesser than n - 1");
		}
	}

	T numerator = static_cast<T>(0), denominator = static_cast<T>(0);

	T rest;

	// Precompute initial Pochhammer symbol terms
	// For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (9.4)
	// Compute: (γ+k+2)_{n-1}/(γ+k+1)_{n} = Γ(γ+k+n+1)/Γ(γ+k+2) × Γ(γ+k+1)/Γ(γ+k+n+1)
	T up = static_cast<T>(1), down = static_cast<T>(1);
	T binomial_coef = binomial_coefficient<T,K>(static_cast<T>(n), static_cast<K>(0));
	T S_n = this->series->Sn(order);

	T down_coef = gamma + static_cast<T>(order + static_cast<K>(2));
	T   up_coef = down_coef - static_cast<T>(n);

	// Compute (γ+k+2)_{n-1} = ∏_{m=0}^{n-2} (γ+k+2+m)
	// Compute (γ+k+1)_{n} = ∏_{m=0}^{n-1} (γ+k+1+m)
	for (K m = static_cast<K>(0); m < n - static_cast<K>(1); ++m) {
		up   *= (up_coef   + static_cast<T>(m));
		down *= (down_coef + static_cast<T>(m));
	}
	up /= down;

	// Update coefficients for the inner product terms
	down_coef = gamma + static_cast<T>(order + static_cast<K>(1));
	up_coef   = down_coef - static_cast<T>(n + static_cast<K>(1));

	// Main summation loop
	// For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (9.2)
	for (K j = static_cast<K>(0); j <= n; ++j) {

		// Compute (-1)^j * C(n,j) * (n-j)
		rest  = minus_one_raised_to_power_n<T,K>(j); 
		rest *= binomial_coef * static_cast<T>(n - j);
		rest *= up;										// Multiply by Pochhammer ratio term
		rest /= static_cast<T>(j + static_cast<K>(1));  // Multiply by 1/(j+1) factor

		up /= (  up_coef + static_cast<T>(j));			// Update Pochhammer ratio for next iteration
		up *= (down_coef + static_cast<T>(j));			// (γ+k+1-j)_{j}/(γ+k+2-n)_{j} → (γ+k+1-j)_{j+1}/(γ+k+2-n)_{j+1}

		// Multiply by remainder term 1/R_{k+j}
		rest *= remainder->operator()(
			order, 
			j, 
			this->series, 
			-gamma-static_cast<T>(n)
		);

		// Accumulate numerator and denominator
		numerator	+= rest * S_n ;
		denominator += rest;

		// Update partial sum for next iteration: S_{k+j+1} = S_{k+j} + a_{k+j+1}
		S_n += this->series->an(order + j + static_cast<K>(1));

		// TODO проверить корректность пересчета бин. коэф.
		//// Update binomial coefficient for next iteration: C(n, j+1) = C(n, j) * (n-j)/(j+1)
		//binomial_coef *= static_cast<T>(n - j);
		//binomial_coef /= static_cast<T>(j + static_cast<K>(1));
	}

	numerator /= denominator;

	if (!isfinite(numerator))
		throw std::overflow_error("division by zero");
	return numerator;
}

template<Accepted T, std::unsigned_integral K, typename series_templ>
T levin_sidi_m_algorithm<T, K, series_templ>::operator()(const K n, const K order) {
	return calculate(n, order);
}

template<Accepted T, std::unsigned_integral K, typename series_templ>
levin_sidi_m_algorithm<T, K, series_templ>::levin_sidi_m_algorithm(
	const series_templ& series, 
	const remainder_type variant, 
	const T gamma_
	) :
	series_acceleration<T, K, series_templ>(series),
	variant(variant),
	gamma(gamma_)
{

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