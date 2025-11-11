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

#include "series_acceleration.hpp"
#include "remainders.hpp"
#include <memory> // For std::unique_ptr

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
template<AcceptedLike T, UnsignedIntLike K>
class levin_sidi_m_algorithm final : public series_acceleration<T, K>
{
protected:

	T gamma_in_use = static_cast<T>(10);											///< Positive real parameter such that gamma >= order - 1
	std::unique_ptr<const transform_base<T, K>> remainder;	///< Pointer to remainder transformation object
	remainder_type remainder_type_in_use;

public:

	/**
	 * @brief Parameterized constructor to initialize the Levin-Sidi M-transformation.
	 *
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 * @param variant Type of remainder transformation to use
	 *        Valid values: u_type, t_type, v_type, t_wave_type, v_wave_type
	 *        Determines the remainder estimate R_n used in the transformation
	 * @param gamma_ Positive real parameter such that gamma >= order - 1, see p. 64 [https://arxiv.org/pdf/math/0306302.pdf]
	 *        Default value: 10.0. Affects the factorial terms in the transformation.
	 *        For theory, see: Sidi (2003, arXiv:math/0306302), p. 64
	 */
	explicit levin_sidi_m_algorithm(
		remainder_type remainder_type_to_use = remainder_type::u_type,
		const T& gamma_to_use = static_cast<T>(10)
	) : series_acceleration<T, K>() { update_gamma(gamma_to_use); update_type(remainder_type_to_use); }

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
	T operator()(
		const K n, 
        const K order, 
        const series_result<T>& data
	) const override;

	void update_type(const remainder_type remainder_type_to_use);
	void update_gamma(const T& new_gamma) { gamma_in_use = new_gamma; }

	std::string get_name() override {

		using std::to_string;

		series_acceleration<T, K>::acceleration_name = "levin sidi m algorithm ";
		switch(remainder_type_in_use){
			case remainder_type::u_type 	: { series_acceleration<T, K>::acceleration_name += "with u-variant "; 		break; }
			case remainder_type::t_type 	: { series_acceleration<T, K>::acceleration_name += "with t-variant "; 		break; }
			case remainder_type::v_type 	: { series_acceleration<T, K>::acceleration_name += "with v-variant "; 		break; }
			case remainder_type::t_wave_type: { series_acceleration<T, K>::acceleration_name += "with t-wave-variant "; break; }
			case remainder_type::v_wave_type: { series_acceleration<T, K>::acceleration_name += "with v-wave-variant "; break; }
		}
		series_acceleration<T, K>::acceleration_name += "and gamma = " + to_string(gamma_in_use);

		return series_acceleration<T, K>::acceleration_name;
	}
};

template<AcceptedLike T, UnsignedIntLike K>
void levin_sidi_m_algorithm<T, K>::update_type(const remainder_type remainder_type_to_use){

	remainder_type_in_use = remainder_type_to_use;

	// Initialize the appropriate remainder transformation based on variant
	switch(remainder_type_to_use){
        case remainder_type::u_type 	: { remainder.reset(new u_transform<T, K>()); 	  break; }
        case remainder_type::t_type 	: { remainder.reset(new t_transform<T, K>()); 	  break; }
        case remainder_type::v_type 	: { remainder.reset(new v_transform<T, K>()); 	  break; }
        case remainder_type::t_wave_type: { remainder.reset(new t_wave_transform<T, K>()); break; }
        case remainder_type::v_wave_type: { remainder.reset(new v_wave_transform<T, K>()); break; }
        default:
		{
			remainder_type_in_use = remainder_type::u_type;
            remainder.reset(new u_transform<T, K>()); // Default to u-variant
		}
    }

}

template<AcceptedLike T, UnsignedIntLike K>
T levin_sidi_m_algorithm<T, K>::operator()(
	const K n, 
    const K order, 
    const series_result<T>& data
) const {

    const K required_size = n + order + static_cast<K>(1) + static_cast<K>(
		remainder_type_in_use == remainder_type::t_wave_type ||
		remainder_type_in_use == remainder_type::v_type ||
		remainder_type_in_use == remainder_type::v_wave_type
	);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for M_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

	using std::isfinite;

	//TODO разобраться с документом (pdf) n/order
    // Validate parameter constraint: gamma >= n - 1

	if(gamma_in_use - static_cast<T>(n - static_cast<K>(1)) < static_cast<T>(0)){
		throw std::domain_error("gamma cannot be lesser than n - 1");
	}

	T numerator   = static_cast<T>(0);
	T denominator = static_cast<T>(0);

	T rest = static_cast<T>(0);

	// Precompute initial Pochhammer symbol terms
	// For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (9.4)
	// Compute: (γ+k+2)_{n-1}/(γ+k+1)_{n} = Γ(γ+k+n+1)/Γ(γ+k+2) × Γ(γ+k+1)/Γ(γ+k+n+1)
	T up = static_cast<T>(1.0);
	T down = static_cast<T>(1.0);
	T binomial_coef = static_cast<T>(binomial_coefficient(n, static_cast<K>(0)));

	T down_coef = gamma_in_use + static_cast<T>(order + static_cast<K>(2));
	T   up_coef = down_coef - static_cast<T>(n);

	// Compute (γ+k+2)_{n-1} = ∏_{m=0}^{n-2} (γ+k+2+m)
	// Compute (γ+k+1)_{n} = ∏_{m=0}^{n-1} (γ+k+1+m)
	for (K m = static_cast<K>(0); m < n - static_cast<K>(1); ++m) {
		up   *= (up_coef   + static_cast<T>(m));
		down *= (down_coef + static_cast<T>(m));
	}
	up /= down;

	// Update coefficients for the inner product terms
	down_coef = gamma_in_use + static_cast<T>(order + static_cast<K>(1));
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
			order + j,
			order + j,
			data.an,
			-gamma_in_use-static_cast<T>(n)
		);

		// Accumulate numerator and denominator
		numerator	+= rest * data.Sn.at(order + j ) ;
		denominator += rest;

		// TODO проверить корректность пересчета бин. коэф.
		//// Update binomial coefficient for next iteration: C(n, j+1) = C(n, j) * (n-j)/(j+1)
		//binomial_coef *= static_cast<T>(n - j);
		//binomial_coef /= static_cast<T>(j + static_cast<K>(1));
	}

	numerator /= denominator;

    if(!isfinite(numerator)){
        throw std::overflow_error("division by zero");
    }

	return numerator;
}

#ifdef INC_FPRECISION

template<UnsignedIntLike K>
class levin_sidi_m_algorithm<float_precision, K> final : public series_acceleration<float_precision, K>
{
protected:

	float_precision gamma_in_use;											///< Positive real parameter such that gamma >= order - 1
	std::unique_ptr<const transform_base<float_precision, K>> remainder;	///< Pointer to remainder transformation object
	remainder_type remainder_type_in_use;

public:

	explicit levin_sidi_m_algorithm(
		remainder_type remainder_type_to_use = remainder_type::u_type,
		const float_precision& gamma_to_use = static_cast<float_precision>(10)
	) : series_acceleration<float_precision, K>() { update_gamma(gamma_to_use); update_type(remainder_type_to_use); }

	float_precision operator()(
		const K n, 
        const K order, 
        const series_result<float_precision>& data
	) const override;

	void update_type(const remainder_type remainder_type_to_use);
	void update_gamma(const float_precision& new_gamma) { gamma_in_use = new_gamma; }

	std::string get_name() override {

		using std::to_string;

		series_acceleration<float_precision, K>::acceleration_name = "levin sidi m algorithm ";
		switch(remainder_type_in_use){
			case remainder_type::u_type 	: { series_acceleration<float_precision, K>::acceleration_name += "with u-variant "; 		break; }
			case remainder_type::t_type 	: { series_acceleration<float_precision, K>::acceleration_name += "with t-variant "; 		break; }
			case remainder_type::v_type 	: { series_acceleration<float_precision, K>::acceleration_name += "with v-variant "; 		break; }
			case remainder_type::t_wave_type: { series_acceleration<float_precision, K>::acceleration_name += "with t-wave-variant "; break; }
			case remainder_type::v_wave_type: { series_acceleration<float_precision, K>::acceleration_name += "with v-wave-variant "; break; }
		}
		series_acceleration<float_precision, K>::acceleration_name += "and gamma = " + to_string(gamma_in_use);
		
		return series_acceleration<float_precision, K>::acceleration_name;
	}

};

template<UnsignedIntLike K>
void levin_sidi_m_algorithm<float_precision, K>::update_type(const remainder_type remainder_type_to_use){

	remainder_type_in_use = remainder_type_to_use;

	// Initialize the appropriate remainder transformation based on variant
	switch(remainder_type_to_use){
        case remainder_type::u_type 	: { remainder.reset(new u_transform<float_precision, K>()); 	  break; }
        case remainder_type::t_type 	: { remainder.reset(new t_transform<float_precision, K>()); 	  break; }
        case remainder_type::v_type 	: { remainder.reset(new v_transform<float_precision, K>()); 	  break; }
        case remainder_type::t_wave_type: { remainder.reset(new t_wave_transform<float_precision, K>()); break; }
        case remainder_type::v_wave_type: { remainder.reset(new v_wave_transform<float_precision, K>()); break; }
        default:
		{
			remainder_type_in_use = remainder_type::u_type;
            remainder.reset(new u_transform<float_precision, K>()); // Default to u-variant
		}
    }
	
}

template<UnsignedIntLike K>
float_precision levin_sidi_m_algorithm<float_precision, K>::operator()(
	const K n, 
    const K order, 
    const series_result<float_precision>& data
) const {

    K required_size = n + order + static_cast<K>(1) + static_cast<K>(
		remainder_type_in_use == remainder_type::t_wave_type ||
		remainder_type_in_use == remainder_type::v_type ||
		remainder_type_in_use == remainder_type::v_wave_type
	);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for M_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

	using std::isfinite;

	//TODO разобраться с документом (pdf) n/order
    // Validate parameter constraint: gamma >= n - 1

	if(gamma_in_use - static_cast<float_precision>(n - static_cast<K>(1)) < static_cast<float_precision>(0)){
		throw std::domain_error("gamma cannot be lesser than n - 1");
	}

	const size_t precision = std::max(data.Sn[0].precision(), data.an[0].precision());

	float_precision numerator   = float_precision(0, precision);
	float_precision denominator = float_precision(0, precision);
	float_precision rest        = float_precision(0, precision);

	// Precompute initial Pochhammer symbol terms
	// For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (9.4)
	// Compute: (γ+k+2)_{n-1}/(γ+k+1)_{n} = Γ(γ+k+n+1)/Γ(γ+k+2) × Γ(γ+k+1)/Γ(γ+k+n+1)
	float_precision up   = float_precision(1, precision);
	float_precision down = float_precision(1, precision);
	float_precision binomial_coef = float_precision(binomial_coefficient(n, static_cast<K>(0)));

	float_precision down_coef = gamma_in_use + float_precision(order + static_cast<K>(2));
	float_precision   up_coef = down_coef - float_precision(n);

	// Compute (γ+k+2)_{n-1} = ∏_{m=0}^{n-2} (γ+k+2+m)
	// Compute (γ+k+1)_{n} = ∏_{m=0}^{n-1} (γ+k+1+m)
	for (K m = static_cast<K>(0); m < n - static_cast<K>(1); ++m) {
		up   *= (up_coef   + static_cast<float_precision>(m));
		down *= (down_coef + static_cast<float_precision>(m));
	}
	up /= down;

	// Update coefficients for the inner product terms
	down_coef = gamma_in_use + static_cast<float_precision>(order + static_cast<K>(1));
	up_coef   = down_coef - static_cast<float_precision>(n + static_cast<K>(1));

	// Main summation loop
	// For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (9.2)
	for (K j = static_cast<K>(0); j <= n; ++j) {

		// Compute (-1)^j * C(n,j) * (n-j)
		rest  = minus_one_raised_to_power_n<float_precision,K>(j);
		rest *= binomial_coef * static_cast<float_precision>(n - j);
		rest *= up;										// Multiply by Pochhammer ratio term
		rest /= static_cast<float_precision>(j + static_cast<K>(1));  // Multiply by 1/(j+1) factor

		up /= (  up_coef + static_cast<float_precision>(j));			// Update Pochhammer ratio for next iteration
		up *= (down_coef + static_cast<float_precision>(j));			// (γ+k+1-j)_{j}/(γ+k+2-n)_{j} → (γ+k+1-j)_{j+1}/(γ+k+2-n)_{j+1}

		// Multiply by remainder term 1/R_{k+j}
		rest *= remainder->operator()(
			order + j,
			order + j,
			data.an,
			-gamma_in_use-static_cast<float_precision>(n)
		);

		// Accumulate numerator and denominator
		numerator	+= rest * data.Sn.at(order + j) ;
		denominator += rest;

		// TODO проверить корректность пересчета бин. коэф.
		//// Update binomial coefficient for next iteration: C(n, j+1) = C(n, j) * (n-j)/(j+1)
		//binomial_coef *= static_cast<T>(n - j);
		//binomial_coef /= static_cast<T>(j + static_cast<K>(1));
	}

	numerator /= denominator;

    if(!isfinite(numerator)){
        throw std::overflow_error("division by zero");
    }

	return numerator;
}

#ifdef INC_COMPLEXPRECISION

template<std::floating_point T, UnsignedIntLike K>
class levin_sidi_m_algorithm<complex_precision<T>, K> final : public series_acceleration<complex_precision<T>, K>
{
protected:

	T gamma_in_use;											///< Positive real parameter such that gamma >= order - 1
	std::unique_ptr<const transform_base<complex_precision<T>, K>> remainder;	///< Pointer to remainder transformation object
	remainder_type remainder_type_in_use;

public:

	/**
	 * @brief Parameterized constructor to initialize the Levin-Sidi M-transformation.
	 *
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 * @param variant Type of remainder transformation to use
	 *        Valid values: u_type, t_type, v_type, t_wave_type, v_wave_type
	 *        Determines the remainder estimate R_n used in the transformation
	 * @param gamma_in_use Positive real parameter such that gamma >= order - 1, see p. 64 [https://arxiv.org/pdf/math/0306302.pdf]
	 *        Default value: 10.0. Affects the factorial terms in the transformation.
	 *        For theory, see: Sidi (2003, arXiv:math/0306302), p. 64
	 */
	explicit levin_sidi_m_algorithm(
		remainder_type remainder_type_in_use = remainder_type::u_type,
		const T& gamma_to_use = static_cast<T>(10)
	) : series_acceleration<complex_precision<T>, K>() { update_gamma(gamma_to_use); update_type(remainder_type_in_use); }

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
	complex_precision<T> operator()(
		const K n, 
        const K order, 
        const series_result<complex_precision<T>>& data
	) const override;

	void update_type(const remainder_type remainder_type_to_use);
	void update_gamma(const T& new_gamma) { gamma_in_use = new_gamma; }

	std::string get_name() override {

		using std::to_string;

		series_acceleration<complex_precision<T>, K>::acceleration_name = "levin sidi m algorithm ";
		switch(remainder_type_in_use){
			case remainder_type::u_type 	: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with u-variant "; 		break; }
			case remainder_type::t_type 	: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with t-variant "; 		break; }
			case remainder_type::v_type 	: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with v-variant "; 		break; }
			case remainder_type::t_wave_type: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with t-wave-variant "; break; }
			case remainder_type::v_wave_type: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with v-wave-variant "; break; }
		}
		series_acceleration<complex_precision<T>, K>::acceleration_name += "and gamma = " + to_string(gamma_in_use);

		return series_acceleration<complex_precision<T>, K>::acceleration_name;
	}
	
};

template<std::floating_point T, UnsignedIntLike K>
void levin_sidi_m_algorithm<complex_precision<T>, K>::update_type(const remainder_type remainder_type_to_use){

	remainder_type_in_use = remainder_type_to_use;

	// Initialize the appropriate remainder transformation based on variant
	switch(remainder_type_to_use){
        case remainder_type::u_type 	: { remainder.reset(new u_transform<complex_precision<T>, K>()); 	  break; }
        case remainder_type::t_type 	: { remainder.reset(new t_transform<complex_precision<T>, K>()); 	  break; }
        case remainder_type::v_type 	: { remainder.reset(new v_transform<complex_precision<T>, K>()); 	  break; }
        case remainder_type::t_wave_type: { remainder.reset(new t_wave_transform<complex_precision<T>, K>()); break; }
        case remainder_type::v_wave_type: { remainder.reset(new v_wave_transform<complex_precision<T>, K>()); break; }
        default:
		{
			remainder_type_in_use = remainder_type::u_type;
            remainder.reset(new u_transform<complex_precision<T>, K>()); // Default to u-variant
		}
    }
	
}

template<std::floating_point T, UnsignedIntLike K>
complex_precision<T> levin_sidi_m_algorithm<complex_precision<T>, K>::operator()(
	const K n, 
    const K order, 
    const series_result<complex_precision<T>>& data
) const {

    const K required_size = n + order + static_cast<K>(1) + static_cast<K>(
		remainder_type_in_use == remainder_type::t_wave_type ||
		remainder_type_in_use == remainder_type::v_type ||
		remainder_type_in_use == remainder_type::v_wave_type
	);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for M_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

	using std::isfinite;

	//TODO разобраться с документом (pdf) n/order
    // Validate parameter constraint: gamma >= n - 1

	if(gamma_in_use - static_cast<T>(n - static_cast<K>(1)) < static_cast<T>(0)){
		throw std::domain_error("gamma cannot be lesser than n - 1");
	}

	complex_precision<T> numerator   = complex_precision<T>(0);
	complex_precision<T> denominator = complex_precision<T>(0);
	complex_precision<T> rest 		 = complex_precision<T>(0);

	// Precompute initial Pochhammer symbol terms
	// For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (9.4)
	// Compute: (γ+k+2)_{n-1}/(γ+k+1)_{n} = Γ(γ+k+n+1)/Γ(γ+k+2) × Γ(γ+k+1)/Γ(γ+k+n+1)
	complex_precision<T> up            = complex_precision<T>(1.0);
	complex_precision<T> down          = complex_precision<T>(1.0);
	complex_precision<T> binomial_coef = complex_precision<T>(binomial_coefficient(n, static_cast<K>(0)));

	complex_precision<T> down_coef = complex_precision<T>(gamma_in_use) + complex_precision<T>(order + static_cast<K>(2));
	complex_precision<T>   up_coef = down_coef - complex_precision<T>(n);

	// Compute (γ+k+2)_{n-1} = ∏_{m=0}^{n-2} (γ+k+2+m)
	// Compute (γ+k+1)_{n} = ∏_{m=0}^{n-1} (γ+k+1+m)
	for (K m = static_cast<K>(0); m < n - static_cast<K>(1); ++m) {
		up   *= (up_coef   + complex_precision<T>(m));
		down *= (down_coef + complex_precision<T>(m));
	}
	up /= down;

	// Update coefficients for the inner product terms
	down_coef = complex_precision<T>(gamma_in_use) + complex_precision<T>(order + static_cast<K>(1));
	up_coef   = down_coef - complex_precision<T>(n + static_cast<K>(1));

	// Main summation loop
	// For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (9.2)
	for (K j = static_cast<K>(0); j <= n; ++j) {

		// Compute (-1)^j * C(n,j) * (n-j)
		rest  = minus_one_raised_to_power_n<complex_precision<T>,K>(j);
		rest *= binomial_coef * complex_precision<T>(n - j);
		rest *= up;										// Multiply by Pochhammer ratio term
		rest /= complex_precision<T>(j + static_cast<K>(1));  // Multiply by 1/(j+1) factor

		up /= (  up_coef + complex_precision<T>(j));			// Update Pochhammer ratio for next iteration
		up *= (down_coef + complex_precision<T>(j));			// (γ+k+1-j)_{j}/(γ+k+2-n)_{j} → (γ+k+1-j)_{j+1}/(γ+k+2-n)_{j+1}

		// Multiply by remainder term 1/R_{k+j}
		rest *= remainder->operator()(
			order + j,
			order + j,
			data.an,
			complex_precision<T>(-gamma_in_use-static_cast<T>(n))
		);

		// Accumulate numerator and denominator
		numerator	+= rest * data.Sn.at(order + j ) ;
		denominator += rest;

		// TODO проверить корректность пересчета бин. коэф.
		//// Update binomial coefficient for next iteration: C(n, j+1) = C(n, j) * (n-j)/(j+1)
		//binomial_coef *= static_cast<T>(n - j);
		//binomial_coef /= static_cast<T>(j + static_cast<K>(1));
	}

	numerator /= denominator;

    if(!isfinite(numerator.real()) || !isfinite(numerator.imag())){
        throw std::overflow_error("division by zero");
    }

	return numerator;
}

template<UnsignedIntLike K>
class levin_sidi_m_algorithm<complex_precision<float_precision>, K> final : public series_acceleration<complex_precision<float_precision>, K>
{
protected:

	float_precision gamma_in_use;											///< Positive real parameter such that gamma >= order - 1
	std::unique_ptr<const transform_base<complex_precision<float_precision>, K>> remainder;	///< Pointer to remainder transformation object
	remainder_type remainder_type_in_use;

public:

	explicit levin_sidi_m_algorithm(
		remainder_type remainder_type_in_use = remainder_type::u_type,
		const float_precision& gamma_to_use = static_cast<float_precision>(10)
	) : series_acceleration<complex_precision<float_precision>, K>() { 
		update_gamma(gamma_to_use); update_type(remainder_type_in_use); 
	}

	complex_precision<float_precision> operator()(
		const K n, 
        const K order, 
        const series_result<complex_precision<float_precision>>& data
	) const override;

	void update_type(const remainder_type remainder_type_to_use);
	void update_gamma(const float_precision& new_gamma) { gamma_in_use = new_gamma; }

	std::string get_name() override {

		using std::to_string;

		using ComplexFloatPrec = complex_precision<float_precision>;

		series_acceleration<ComplexFloatPrec, K>::acceleration_name = "levin sidi m algorithm ";
		switch(remainder_type_in_use){
			case remainder_type::u_type 	: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with u-variant "; 		break; }
			case remainder_type::t_type 	: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with t-variant "; 		break; }
			case remainder_type::v_type 	: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with v-variant "; 		break; }
			case remainder_type::t_wave_type: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with t-wave-variant "; break; }
			case remainder_type::v_wave_type: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with v-wave-variant "; break; }
		}
		series_acceleration<ComplexFloatPrec, K>::acceleration_name += "and gamma = " + to_string(gamma_in_use);

		return series_acceleration<ComplexFloatPrec, K>::acceleration_name;
	}
};

template<UnsignedIntLike K>
void levin_sidi_m_algorithm<complex_precision<float_precision>, K>::update_type(const remainder_type remainder_type_to_use){

	remainder_type_in_use = remainder_type_to_use;

	using ComplexFloatPrec = complex_precision<float_precision>;

	// Initialize the appropriate remainder transformation based on variant
	switch(remainder_type_to_use){
        case remainder_type::u_type 	: { remainder.reset(new u_transform<ComplexFloatPrec, K>()); 	  break; }
        case remainder_type::t_type 	: { remainder.reset(new t_transform<ComplexFloatPrec, K>()); 	  break; }
        case remainder_type::v_type 	: { remainder.reset(new v_transform<ComplexFloatPrec, K>()); 	  break; }
        case remainder_type::t_wave_type: { remainder.reset(new t_wave_transform<ComplexFloatPrec, K>()); break; }
        case remainder_type::v_wave_type: { remainder.reset(new v_wave_transform<ComplexFloatPrec, K>()); break; }
        default:
		{
			remainder_type_in_use = remainder_type::u_type;
            remainder.reset(new u_transform<ComplexFloatPrec, K>()); // Default to u-variant
		}
    }
	
}

template<UnsignedIntLike K>
complex_precision<float_precision> levin_sidi_m_algorithm<complex_precision<float_precision>, K>::operator()(
	const K n, 
    const K order, 
    const series_result<complex_precision<float_precision>>& data
) const {

    const K required_size = n + order + static_cast<K>(1) + static_cast<K>(
		remainder_type_in_use == remainder_type::t_wave_type ||
		remainder_type_in_use == remainder_type::v_type ||
		remainder_type_in_use == remainder_type::v_wave_type
	);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for M_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

	using std::isfinite;

	//TODO разобраться с документом (pdf) n/order
    // Validate parameter constraint: gamma >= n - 1

	if(gamma_in_use - static_cast<float_precision>(n - static_cast<K>(1)) < static_cast<float_precision>(0)){
		throw std::domain_error("gamma cannot be lesser than n - 1");
	}

	using Complex = complex_precision<float_precision>;

	const size_t precision = std::max(
		std::max(data.Sn[0].real().precision(),data.Sn[0].imag().precision()) ,
		std::max(data.an[0].real().precision(),data.an[0].imag().precision())
	);

	Complex numerator   = Complex(float_precision(0, precision), float_precision(0, precision));
	Complex denominator = Complex(float_precision(0, precision), float_precision(0, precision));
	Complex rest        = Complex(float_precision(0, precision), float_precision(0, precision));

	// Precompute initial Pochhammer symbol terms
	// For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (9.4)
	// Compute: (γ+k+2)_{n-1}/(γ+k+1)_{n} = Γ(γ+k+n+1)/Γ(γ+k+2) × Γ(γ+k+1)/Γ(γ+k+n+1)
	Complex up   = Complex(float_precision(1, precision), float_precision(0, precision));
	Complex down = Complex(float_precision(1, precision), float_precision(0, precision));
	Complex binomial_coef = Complex(binomial_coefficient(n, static_cast<K>(0)));

	Complex down_coef = Complex(gamma_in_use) + Complex(order + static_cast<K>(2));
	Complex   up_coef = down_coef - Complex(n);

	// Compute (γ+k+2)_{n-1} = ∏_{m=0}^{n-2} (γ+k+2+m)
	// Compute (γ+k+1)_{n} = ∏_{m=0}^{n-1} (γ+k+1+m)
	for (K m = static_cast<K>(0); m < n - static_cast<K>(1); ++m) {
		up   *= (up_coef   + Complex(m));
		down *= (down_coef + Complex(m));
	}
	up /= down;

	// Update coefficients for the inner product terms
	down_coef = Complex(gamma_in_use) + Complex(order + static_cast<K>(1));
	up_coef   = down_coef - Complex(n + static_cast<K>(1));

	// Main summation loop
	// For theory, see: Sidi (2003, arXiv:math/0306302), Eq. (9.2)
	for (K j = static_cast<K>(0); j <= n; ++j) {

		// Compute (-1)^j * C(n,j) * (n-j)
		rest  = minus_one_raised_to_power_n<Complex,K>(j);
		rest *= binomial_coef * Complex(n - j);
		rest *= up;										// Multiply by Pochhammer ratio term
		rest /= Complex(j + static_cast<K>(1));  // Multiply by 1/(j+1) factor

		up /= (  up_coef + Complex(j));			// Update Pochhammer ratio for next iteration
		up *= (down_coef + Complex(j));			// (γ+k+1-j)_{j}/(γ+k+2-n)_{j} → (γ+k+1-j)_{j+1}/(γ+k+2-n)_{j+1}

		// Multiply by remainder term 1/R_{k+j}
		rest *= remainder->operator()(
			order + j,
			order + j,
			data.an,
			-Complex(gamma_in_use)-Complex(n)
		);

		// Accumulate numerator and denominator
		numerator	+= rest * data.Sn.at(order + j ) ;
		denominator += rest;

		// TODO проверить корректность пересчета бин. коэф.
		//// Update binomial coefficient for next iteration: C(n, j+1) = C(n, j) * (n-j)/(j+1)
		//binomial_coef *= static_cast<T>(n - j);
		//binomial_coef /= static_cast<T>(j + static_cast<K>(1));
	}

	numerator /= denominator;

    if(!isfinite(numerator.real()) || !isfinite(numerator.imag())){
        throw std::overflow_error("division by zero");
    }

	return numerator;
}

#endif
#endif