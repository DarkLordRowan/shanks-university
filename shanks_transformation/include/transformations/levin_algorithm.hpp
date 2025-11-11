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

#include "series_acceleration.hpp"
#include "remainders.hpp"

#include <cmath> //Include for pow, isfinite
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
template <AcceptedLike T, UnsignedIntLike K>
class levin_algorithm final : public series_acceleration<T, K>
{
protected:

	T beta_in_use = static_cast<T>(1);													///< Parameter for u-variant transformation (β > 0). Default value is 1.0.
    std::unique_ptr<const transform_base<T, K>> remainder;			//< Pointer to remainder transformation object
    bool use_recurrent_formula = false;								//< Flag to use recurrence formulas (true) or direct formulas (false)
    remainder_type remainder_type_in_use = remainder_type::u_type;	//< Type of Levin transformation variant (u, t, v, t~, v~)

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
	inline T calc_result(
		const K n, 
        const K order, 
        const series_result<T>& data
	) const;

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
	inline T calc_result_rec(
		const K n, 
        const K order, 
        const series_result<T>& data
	) const;

public:

	/**
	 * @brief Parameterized constructor to initialize the Levin Algorithm.
	 *
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 * @param remainder_type_to_use Type of Levin transformation variant to use
	 *        Valid values: u_type, t_type, v_type, t_wave_type, v_wave_type
	 *        Determines the remainder estimate R_n used in the transformation
	 * @param use_recurrent_formula Flag to use recurrence formulas instead of direct summation
	 *        true: use recursive implementation, false: use direct summation
	 * @param beta_to_use Parameter for u-variant transformation (must be > 0)
	 *        Default value: 1.0. Affects the remainder estimate in u-variant.
	 *        For theory, see: Sidi & Levin (1981), Eq. (3.4) and surrounding discussion
	 */
	explicit levin_algorithm(
        const remainder_type remainder_type_to_use = remainder_type::u_type,
        const bool use_recurrent_formula = false,
        const T& beta_to_use = static_cast<T>(1)
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
	T operator()( 
        const K n, 
        const K order, 
        const series_result<T>& data
	) const override;

	void update_beta(const T& new_beta){
		beta_in_use = (new_beta > static_cast<T>(0) ? new_beta : static_cast<T>(1));
	}

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

	std::string get_name() override {

		using std::to_string;

		series_acceleration<T, K>::acceleration_name = (use_recurrent_formula ? "recurrent " : "");
		series_acceleration<T, K>::acceleration_name += "levin l algorithm ";
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
levin_algorithm<T, K>::levin_algorithm(
    const remainder_type remainder_type_to_use,
    const bool use_recurrent_formula,
    const T& beta_to_use
) :
	series_acceleration<T, K>(),
	use_recurrent_formula(use_recurrent_formula)
{//TODO: нужно ли проверять бету на допустимость?

	update_beta(beta_to_use);
	// Initialize the appropriate remainder transformation based on variant
    update_type(remainder_type_to_use);
}

template<AcceptedLike T, UnsignedIntLike K>
inline T levin_algorithm<T, K>::calc_result(
	const K n, 
    const K order, 
    const series_result<T>& data
) const {

	using std::pow;
	using std::isfinite;

	T numerator   = static_cast<T>(0);
	T denominator = static_cast<T>(0);
	T C_njk 	  = static_cast<T>(0);
	T rest 		  = static_cast<T>(0);

	// For theory, see: Levin (1973), Eq. (2.3)
	// T_{k,n} = [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} S_{n+j}/R_{n+j}] /
	//           [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} 1/R_{n+j}]
	for (K j = static_cast<K>(0); j <= order; ++j) {
		// Compute (-1)^j * C(k,j)
		rest  = minus_one_raised_to_power_n<T,K>(j);
		rest *= static_cast<T>(binomial_coefficient<K>(order, j));

		// Compute (n+j+1)^{k-1}/(n+k+1)^{k-1}
		C_njk  = pow(beta_in_use + static_cast<T>(n + j     + static_cast<K>(1)), static_cast<T>(order - static_cast<K>(1)));
		C_njk /= pow(beta_in_use + static_cast<T>(n + order + static_cast<K>(1)), static_cast<T>(order - static_cast<K>(1)));

		// Compute 1/R_{n+j} where R_{n+j} is the remainder estimate
		rest*= remainder->operator()(
            n + j,
            n + j,
            data.an,
            (remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<T>(1))
        );

		rest *= C_njk;

		denominator += rest;
		numerator += rest * data.Sn.at(n + j);
	}

	numerator /= denominator;

	if (!isfinite(numerator))
		throw std::overflow_error("division by zero");

	return numerator;
}

template<AcceptedLike T, UnsignedIntLike K>
inline T levin_algorithm<T, K>::calc_result_rec(
	const K n, 
    const K order, 
    const series_result<T>& data
) const{

	using std::isfinite;
	using std::pow;

	// For theory, see: Sidi (1979), Section 3 - Recursive implementation using E-algorithm
	// Initialize arrays for recursive computation
	std::vector<T>   Num(
		order + static_cast<K>(1), 
		static_cast<T>(0)
	);
	std::vector<T> Denom(
		order + static_cast<K>(1), 
		static_cast<T>(0)
	);

	// Initialize base values: E_0^{(n)} = S_n, g_0^{(n)} = 1/R_n
	for (K i = static_cast<K>(0); i < order+static_cast<K>(1); ++i) {
		Denom[i] = remainder->operator()(
            n+i,
            n+i,
            data.an,
            (remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<T>(1))
        );

		Num[i] = data.Sn.at(n+i) * Denom[i];
	}

	// Recursive computation using the E-algorithm scheme
	T scale = static_cast<T>(0);
	for (K i = static_cast<K>(1); i <= order; ++i)
		for (K j = static_cast<K>(0); j <= order - i; ++j) {

			// For theory, see: Brezinski's E-algorithm recurrence
			// E_k^{(n)} = E_{k-1}^{(n)} - g_{k-1,k}^{(n)} * ΔE_{k-1}^{(n)} / Δg_{k-1,k}^{(n)}
			scale = (beta_in_use + static_cast<T>(n + j));
			scale*= pow(static_cast<T>(1) - static_cast<T>(1) / (beta_in_use + static_cast<T>(n + j + i + 1)), static_cast<T>(i));
			scale/= (beta_in_use + static_cast<T>(n + j + i));

			Denom[j] = fma(scale,Denom[j],Denom[j+static_cast<K>(1)]);
              Num[j] = fma(scale,  Num[j],  Num[j+static_cast<K>(1)]);
		}

	Num[0] /= Denom[0];

	if (!isfinite(Num[0]))
		throw std::overflow_error("division by zero");

	return Num[0];
}

template <AcceptedLike T, UnsignedIntLike K>
T levin_algorithm<T, K>::operator()(
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
        throw std::out_of_range("The Sn or an smaller then required for L_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
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

template <UnsignedIntLike K>
class levin_algorithm<float_precision, K> final : public series_acceleration<float_precision, K>
{
protected:

	float_precision beta_in_use = float_precision(1);													///< Parameter for u-variant transformation (β > 0). Default value is 1.0.
    std::unique_ptr<const transform_base<float_precision, K>> remainder;	///< Pointer to remainder transformation object
    bool use_recurrent_formula = false;							///< Flag to use recurrence formulas (true) or direct formulas (false)
    remainder_type remainder_type_in_use = remainder_type::u_type;		///< Type of Levin transformation variant (u, t, v, t~, v~)

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
	inline float_precision calc_result(
		const K n, 
        const K order, 
        const series_result<float_precision>& data
	) const;

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
	inline float_precision calc_result_rec(
		const K n, 
        const K order, 
        const series_result<float_precision>& data
	) const;

public:

	/**
	 * @brief Parameterized constructor to initialize the Levin Algorithm.
	 *
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 * @param remainder_type_to_use Type of Levin transformation variant to use
	 *        Valid values: u_type, t_type, v_type, t_wave_type, v_wave_type
	 *        Determines the remainder estimate R_n used in the transformation
	 * @param use_recurrent_formula Flag to use recurrence formulas instead of direct summation
	 *        true: use recursive implementation, false: use direct summation
	 * @param beta_to_use Parameter for u-variant transformation (must be > 0)
	 *        Default value: 1.0. Affects the remainder estimate in u-variant.
	 *        For theory, see: Sidi & Levin (1981), Eq. (3.4) and surrounding discussion
	 */
	explicit levin_algorithm(
        const remainder_type remainder_type_to_use = remainder_type::u_type,
        const bool use_recurrent_formula = false,
        const float_precision& beta_to_use = static_cast<float_precision>(1)
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
	float_precision operator()( 
        const K n, 
        const K order, 
        const series_result<float_precision>& data
	) const override;

	void update_beta(const float_precision& new_beta){
		beta_in_use = (new_beta > static_cast<float_precision>(0) ? new_beta : static_cast<float_precision>(1));
	}

	void update_type(const remainder_type remainder_type_to_use){

		remainder_type_in_use = remainder_type_to_use;

		switch(remainder_type_to_use){
        	case remainder_type::u_type 	: { remainder.reset(new u_transform<float_precision, K>()	 ); break; }
        	case remainder_type::t_type 	: { remainder.reset(new t_transform<float_precision, K>()	 ); break; }
        	case remainder_type::v_type 	: { remainder.reset(new v_transform<float_precision, K>()	 ); break; }
        	case remainder_type::t_wave_type: { remainder.reset(new t_wave_transform<float_precision, K>()); break; }
        	case remainder_type::v_wave_type: { remainder.reset(new v_wave_transform<float_precision, K>()); break; }
        	default:{
				remainder_type_in_use = remainder_type::u_type;
        	    remainder.reset(new u_transform<float_precision, K>()); // Default to u-variant
			}
		}
	}

	std::string get_name() override {

		using std::to_string;

		series_acceleration<float_precision, K>::acceleration_name = (use_recurrent_formula ? "recurrent " : "");
		series_acceleration<float_precision, K>::acceleration_name += "levin l algorithm ";
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
levin_algorithm<float_precision, K>::levin_algorithm(
    const remainder_type remainder_type_to_use,
    const bool use_recurrent_formula,
    const float_precision& beta_in_use
) :
	series_acceleration<float_precision, K>(),
	use_recurrent_formula(use_recurrent_formula)
{//TODO: нужно ли проверять бету на допустимость?

	update_beta(beta_in_use);
	// Initialize the appropriate remainder transformation based on variant
    update_type(remainder_type_to_use);
}

template<UnsignedIntLike K>
inline float_precision levin_algorithm<float_precision, K>::calc_result(
	const K n, 
    const K order, 
    const series_result<float_precision>& data
) const {

	using std::pow;
	using std::isfinite;

	const size_t precision = std::max(data.Sn[0].precision(), data.an[0].precision());

	float_precision numerator   = float_precision(0, precision);
	float_precision denominator = float_precision(0, precision);
	float_precision C_njk 	    = float_precision(0, precision);
	float_precision rest 		= float_precision(0, precision);

	// For theory, see: Levin (1973), Eq. (2.3)
	// T_{k,n} = [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} S_{n+j}/R_{n+j}] /
	//           [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} 1/R_{n+j}]
	for (K j = static_cast<K>(0); j <= order; ++j) {
		// Compute (-1)^j * C(k,j)
		rest  = minus_one_raised_to_power_n<float_precision,K>(j);
		rest *= static_cast<float_precision>(binomial_coefficient<K>(order, j));

		// Compute (n+j+1)^{k-1}/(n+k+1)^{k-1}
		C_njk  = pow(beta_in_use + static_cast<float_precision>(n + j     + static_cast<K>(1)), static_cast<float_precision>(order - static_cast<K>(1)));
		C_njk /= pow(beta_in_use + static_cast<float_precision>(n + order + static_cast<K>(1)), static_cast<float_precision>(order - static_cast<K>(1)));

		// Compute 1/R_{n+j} where R_{n+j} is the remainder estimate
		rest*= remainder->operator()(
            n + j,
            n + j,
            data.an,
            (remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<float_precision>(1))
        );

		rest *= C_njk;

		denominator += rest;
		numerator += rest * data.Sn.at(n + j);
	}

	numerator /= denominator;

	return numerator;
}

template<UnsignedIntLike K>
inline float_precision levin_algorithm<float_precision, K>::calc_result_rec(
	const K n, 
    const K order, 
    const series_result<float_precision>& data
) const{

	using std::isfinite;
	using std::pow;

	const size_t precision = std::max(data.Sn[0].precision(), data.an[0].precision());

	// For theory, see: Sidi (1979), Section 3 - Recursive implementation using E-algorithm
	// Initialize arrays for recursive computation
	std::vector<float_precision>   Num(
		order + static_cast<K>(1), 
		float_precision(0, precision)
	);
	std::vector<float_precision> Denom(
		order + static_cast<K>(1), 
		float_precision(0, precision)
	);

	// Initialize base values: E_0^{(n)} = S_n, g_0^{(n)} = 1/R_n
	for (K i = static_cast<K>(0); i < order+static_cast<K>(1); ++i) {
		Denom[i] = remainder->operator()(
            n + i,
            n + i,
            data.an,
            (remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<float_precision>(1))
        );

		Num[i] = data.Sn.at(n+i) * Denom[i];
	}

	// Recursive computation using the E-algorithm scheme
	float_precision scale = float_precision(0, precision);
	for (K i = static_cast<K>(1); i <= order; ++i)
		for (K j = static_cast<K>(0); j <= order - i; ++j) {

			// For theory, see: Brezinski's E-algorithm recurrence
			// E_k^{(n)} = E_{k-1}^{(n)} - g_{k-1,k}^{(n)} * ΔE_{k-1}^{(n)} / Δg_{k-1,k}^{(n)}
			scale = (beta_in_use + float_precision(n + j));
			scale*= pow(float_precision(1) - float_precision(1) / (beta_in_use + float_precision(n + j + i + 1)), float_precision(i));
			scale/= (beta_in_use + float_precision(n + j + i));

			Denom[j] = fma(-scale,Denom[j],Denom[j+static_cast<K>(1)]);
              Num[j] = fma(-scale,  Num[j],  Num[j+static_cast<K>(1)]);
		}

	Num[0] /= Denom[0];

	return Num[0];
}

template <UnsignedIntLike K>
float_precision levin_algorithm<float_precision, K>::operator()(
	const K n, 
    const K order, 
    const series_result<float_precision>& data
) const {

    const K required_size = n + order + static_cast<K>(1) + static_cast<K>(
		remainder_type_in_use == remainder_type::t_wave_type ||
		remainder_type_in_use == remainder_type::v_type ||
		remainder_type_in_use == remainder_type::v_wave_type
	);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for L_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
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

template <std::floating_point T, UnsignedIntLike K>
class levin_algorithm<complex_precision<T>, K> final : public series_acceleration<complex_precision<T>, K>
{
protected:

	T beta_in_use = static_cast<T>(1);													///< Parameter for u-variant transformation (β > 0). Default value is 1.0.
    std::unique_ptr<const transform_base<complex_precision<T>, K>> remainder;	///< Pointer to remainder transformation object
    bool use_recurrent_formula = false;							///< Flag to use recurrence formulas (true) or direct formulas (false)
    remainder_type remainder_type_in_use = remainder_type::u_type;		///< Type of Levin transformation variant (u, t, v, t~, v~)

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
	inline complex_precision<T> calc_result(
		const K n, 
        const K order, 
         
        const series_result<complex_precision<T>>& data
	) const;

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
	inline complex_precision<T> calc_result_rec(
		const K n, 
        const K order, 
        const series_result<complex_precision<T>>& data
	) const;

public:

	/**
	 * @brief Parameterized constructor to initialize the Levin Algorithm.
	 *
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 * @param remainder_type_to_use Type of Levin transformation variant to use
	 *        Valid values: u_type, t_type, v_type, t_wave_type, v_wave_type
	 *        Determines the remainder estimate R_n used in the transformation
	 * @param use_recurrent_formula Flag to use recurrence formulas instead of direct summation
	 *        true: use recursive implementation, false: use direct summation
	 * @param beta_to_use Parameter for u-variant transformation (must be > 0)
	 *        Default value: 1.0. Affects the remainder estimate in u-variant.
	 *        For theory, see: Sidi & Levin (1981), Eq. (3.4) and surrounding discussion
	 */
	explicit levin_algorithm(
        const remainder_type remainder_type_to_use = remainder_type::u_type,
        const bool use_recurrent_formula = false,
        const T& beta_to_use = static_cast<float_precision>(1)
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
	complex_precision<T> operator()( 
        const K n, 
        const K order, 
        const series_result<complex_precision<T>>& data
	) const override;

	void update_beta(const T& new_beta){
		beta_in_use = (new_beta > static_cast<T>(0) ? new_beta : static_cast<T>(1));
	}

	void update_type(const remainder_type remainder_type_to_use){

		remainder_type_in_use = remainder_type_to_use;

		switch(remainder_type_to_use){
        	case remainder_type::u_type 	: { remainder.reset(new u_transform<complex_precision<T>, K>()	 ); break; }
        	case remainder_type::t_type 	: { remainder.reset(new t_transform<complex_precision<T>, K>()	 ); break; }
        	case remainder_type::v_type 	: { remainder.reset(new v_transform<complex_precision<T>, K>()	 ); break; }
        	case remainder_type::t_wave_type: { remainder.reset(new t_wave_transform<complex_precision<T>, K>()); break; }
        	case remainder_type::v_wave_type: { remainder.reset(new v_wave_transform<complex_precision<T>, K>()); break; }
        	default:{
				remainder_type_in_use = remainder_type::u_type;
        	    remainder.reset(new u_transform<complex_precision<T>, K>()); // Default to u-variant
			}
		}
	}

	std::string get_name() override {

		using std::to_string;

		series_acceleration<complex_precision<T>, K>::acceleration_name = (use_recurrent_formula ? "recurrent " : "");
		series_acceleration<complex_precision<T>, K>::acceleration_name += "levin l algorithm ";
		switch(remainder_type_in_use){
			case remainder_type::u_type 	: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with u-variant "; 	   break; }
			case remainder_type::t_type 	: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with t-variant "; 	   break; }
			case remainder_type::v_type 	: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with v-variant "; 	   break; }
			case remainder_type::t_wave_type: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with t-wave-variant "; break; }
			case remainder_type::v_wave_type: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with v-wave-variant "; break; }
		}
		series_acceleration<complex_precision<T>, K>::acceleration_name += "and beta = " + to_string(beta_in_use);

		return series_acceleration<complex_precision<T>, K>::acceleration_name;
	}
};

template<std::floating_point T, UnsignedIntLike K>
levin_algorithm<complex_precision<T>, K>::levin_algorithm(
    const remainder_type remainder_type_to_use,
    const bool use_recurrent_formula,
    const T& beta_to_use
) :
	series_acceleration<complex_precision<T>, K>(),
	use_recurrent_formula(use_recurrent_formula)
{//TODO: нужно ли проверять бету на допустимость?

	update_beta(beta_to_use);
	// Initialize the appropriate remainder transformation based on variant
    update_type(remainder_type_to_use);
}

template<std::floating_point T, UnsignedIntLike K>
inline complex_precision<T> levin_algorithm<complex_precision<T>, K>::calc_result(
	const K n, 
    const K order, 
    const series_result<complex_precision<T>>& data
) const {

	using std::pow;
	using std::isfinite;

	complex_precision<T> numerator   = static_cast<complex_precision<T>>(0);
	complex_precision<T> denominator = static_cast<complex_precision<T>>(0);
	complex_precision<T> C_njk 	     = static_cast<complex_precision<T>>(0);
	complex_precision<T> rest 		 = static_cast<complex_precision<T>>(0);

	// For theory, see: Levin (1973), Eq. (2.3)
	// T_{k,n} = [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} S_{n+j}/R_{n+j}] /
	//           [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} 1/R_{n+j}]
	for (K j = static_cast<K>(0); j <= order; ++j) {
		// Compute (-1)^j * C(k,j)
		rest  = minus_one_raised_to_power_n<complex_precision<T>,K>(j);
		rest *= static_cast<complex_precision<T>>(binomial_coefficient<K>(order, j));

		// Compute (n+j+1)^{k-1}/(n+k+1)^{k-1}
		C_njk  = static_cast<complex_precision<T>>(
			pow(beta_in_use + static_cast<T>(n + j     + static_cast<K>(1)), 
			static_cast<T>(order - static_cast<K>(1)))
		);
		C_njk /= static_cast<complex_precision<T>>(
			pow(beta_in_use + static_cast<T>(n + order + static_cast<K>(1)), 
			static_cast<T>(order - static_cast<K>(1)))
		);

		// Compute 1/R_{n+j} where R_{n+j} is the remainder estimate
		rest*= remainder->operator()(
            n + j,
            n + j,
            data.an,
            (remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<complex_precision<T>>(1))
        );

		rest *= C_njk;

		denominator += rest;
		numerator += rest * data.Sn.at(n + j);
	}

	numerator /= denominator;

	return numerator;
}

template<std::floating_point T, UnsignedIntLike K>
inline complex_precision<T> levin_algorithm<complex_precision<T>, K>::calc_result_rec(
	const K n, 
    const K order, 
    const series_result<complex_precision<T>>& data
) const{

	using std::isfinite;
	using std::pow;

	// For theory, see: Sidi (1979), Section 3 - Recursive implementation using E-algorithm
	// Initialize arrays for recursive computation
	std::vector<complex_precision<T>>   Num(
		order + static_cast<K>(1), 
		static_cast<complex_precision<T>>(0)
	);
	std::vector<complex_precision<T>> Denom(
		order + static_cast<K>(1), 
		static_cast<complex_precision<T>>(0)
	);

	// Initialize base values: E_0^{(n)} = S_n, g_0^{(n)} = 1/R_n
	for (K i = static_cast<K>(0); i < order+static_cast<K>(1); ++i) {
		Denom[i] = remainder->operator()(
            n + i,
            n + i,
            data.an,
            (remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<complex_precision<T>>(1))
        );

		Num[i] = data.Sn.at(n+i) * Denom[i];
	}

	// Recursive computation using the E-algorithm scheme
	T scale = static_cast<T>(0);
	for (K i = static_cast<K>(1); i <= order; ++i)
		for (K j = static_cast<K>(0); j <= order - i; ++j) {

			// For theory, see: Brezinski's E-algorithm recurrence
			// E_k^{(n)} = E_{k-1}^{(n)} - g_{k-1,k}^{(n)} * ΔE_{k-1}^{(n)} / Δg_{k-1,k}^{(n)}
			scale = (beta_in_use + static_cast<T>(n + j));
			scale*= pow(static_cast<T>(1) - static_cast<T>(1) / (beta_in_use + static_cast<T>(n + j + i + 1)), static_cast<T>(i));
			scale/= (beta_in_use + static_cast<T>(n + j + i));

			Denom[j] = fma(complex_precision<T>(-scale),Denom[j],Denom[j+static_cast<K>(1)]);
              Num[j] = fma(complex_precision<T>(-scale),  Num[j],  Num[j+static_cast<K>(1)]);
		}

	Num[0] /= Denom[0];

	return Num[0];
}

template <std::floating_point T, UnsignedIntLike K>
complex_precision<T> levin_algorithm<complex_precision<T>, K>::operator()(
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
        throw std::out_of_range("The Sn or an smaller then required for L_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
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

template <UnsignedIntLike K>
class levin_algorithm<complex_precision<float_precision>, K> final : public series_acceleration<complex_precision<float_precision>, K>
{
protected:

	float_precision beta_in_use = float_precision(1);													///< Parameter for u-variant transformation (β > 0). Default value is 1.0.
    std::unique_ptr<const transform_base<complex_precision<float_precision>, K>> remainder;	///< Pointer to remainder transformation object
    bool use_recurrent_formula = false;							///< Flag to use recurrence formulas (true) or direct formulas (false)
    remainder_type remainder_type_in_use = remainder_type::u_type;		///< Type of Levin transformation variant (u, t, v, t~, v~)

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
	inline complex_precision<float_precision> calc_result(
		const K n, 
        const K order, 
        const series_result<complex_precision<float_precision>>& data
	) const;

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
	inline complex_precision<float_precision> calc_result_rec(
		const K n, 
        const K order, 
        const series_result<complex_precision<float_precision>>& data
	) const;

public:

	/**
	 * @brief Parameterized constructor to initialize the Levin Algorithm.
	 *
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 * @param remainder_type_to_use Type of Levin transformation variant to use
	 *        Valid values: u_type, t_type, v_type, t_wave_type, v_wave_type
	 *        Determines the remainder estimate R_n used in the transformation
	 * @param use_recurrent_formula Flag to use recurrence formulas instead of direct summation
	 *        true: use recursive implementation, false: use direct summation
	 * @param beta_to_use Parameter for u-variant transformation (must be > 0)
	 *        Default value: 1.0. Affects the remainder estimate in u-variant.
	 *        For theory, see: Sidi & Levin (1981), Eq. (3.4) and surrounding discussion
	 */
	explicit levin_algorithm(
        const remainder_type remainder_type_to_use = remainder_type::u_type,
        const bool use_recurrent_formula = false,
        const float_precision& beta_to_use = static_cast<float_precision>(1)
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
	complex_precision<float_precision> operator()( 
        const K n, 
        const K order, 
        const series_result<complex_precision<float_precision>>& data
	) const override;

	void update_beta(const float_precision& new_beta){
		beta_in_use = (new_beta > float_precision(0) ? new_beta : float_precision(1));
	}

	void update_type(const remainder_type remainder_type_to_use){

		remainder_type_in_use = remainder_type_to_use;

		using ComplexFloatPrec = complex_precision<float_precision>;

		switch(remainder_type_to_use){
        	case remainder_type::u_type 	: { remainder.reset(new u_transform<ComplexFloatPrec, K>()	 ); break; }
        	case remainder_type::t_type 	: { remainder.reset(new t_transform<ComplexFloatPrec, K>()	 ); break; }
        	case remainder_type::v_type 	: { remainder.reset(new v_transform<ComplexFloatPrec, K>()	 ); break; }
        	case remainder_type::t_wave_type: { remainder.reset(new t_wave_transform<ComplexFloatPrec, K>()); break; }
        	case remainder_type::v_wave_type: { remainder.reset(new v_wave_transform<ComplexFloatPrec, K>()); break; }
        	default:{
				remainder_type_in_use = remainder_type::u_type;
        	    remainder.reset(new u_transform<ComplexFloatPrec, K>()); // Default to u-variant
			}
		}
	}

	std::string get_name() override {

		using std::to_string;

		using ComplexFloatPrec = complex_precision<float_precision>;

		series_acceleration<ComplexFloatPrec, K>::acceleration_name = (use_recurrent_formula ? "recurrent " : "");
		series_acceleration<ComplexFloatPrec, K>::acceleration_name += "levin l algorithm ";
		switch(remainder_type_in_use){
			case remainder_type::u_type 	: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with u-variant "; 	   break; }
			case remainder_type::t_type 	: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with t-variant "; 	   break; }
			case remainder_type::v_type 	: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with v-variant "; 	   break; }
			case remainder_type::t_wave_type: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with t-wave-variant "; break; }
			case remainder_type::v_wave_type: { series_acceleration<ComplexFloatPrec, K>::acceleration_name += "with v-wave-variant "; break; }
		}
		series_acceleration<ComplexFloatPrec, K>::acceleration_name += "and beta = " + to_string(beta_in_use);

		return series_acceleration<ComplexFloatPrec, K>::acceleration_name;
	}

};

template<UnsignedIntLike K>
levin_algorithm<complex_precision<float_precision>, K>::levin_algorithm(
    const remainder_type remainder_type_to_use,
    const bool use_recurrent_formula,
    const float_precision& beta_to_use
) :
	series_acceleration<complex_precision<float_precision>, K>(),
	use_recurrent_formula(use_recurrent_formula)
{//TODO: нужно ли проверять бету на допустимость?

	update_beta(beta_to_use);
	// Initialize the appropriate remainder transformation based on variant
    update_type(remainder_type_to_use);
}

template<UnsignedIntLike K>
inline complex_precision<float_precision> levin_algorithm<complex_precision<float_precision>, K>::calc_result(
	const K n, 
    const K order, 
    const series_result<complex_precision<float_precision>>& data
) const {

	using std::pow;
	using std::isfinite;

	const size_t precision = std::max(
		std::max(data.Sn[0].real().precision(),data.Sn[0].imag().precision()), 
		std::max(data.an[0].real().precision(),data.an[0].imag().precision())
	);

	complex_precision<float_precision> numerator   = complex_precision<float_precision>(
		float_precision(0, precision),
		float_precision(0, precision)
	);
	complex_precision<float_precision> denominator = complex_precision<float_precision>(
		float_precision(0, precision),
		float_precision(0, precision)
	);
	complex_precision<float_precision> C_njk 	   = complex_precision<float_precision>(
		float_precision(0, precision),
		float_precision(0, precision)
	);
	complex_precision<float_precision> rest 	   = complex_precision<float_precision>(
		float_precision(0, precision),
		float_precision(0, precision)
	);

	// For theory, see: Levin (1973), Eq. (2.3)
	// T_{k,n} = [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} S_{n+j}/R_{n+j}] /
	//           [∑_{j=0}^k (-1)^j C(k,j) (n+j+1)^{k-1}/(n+k+1)^{k-1} 1/R_{n+j}]
	for (K j = static_cast<K>(0); j <= order; ++j) {
		// Compute (-1)^j * C(k,j)
		rest  = minus_one_raised_to_power_n<complex_precision<float_precision>,K>(j);
		rest *= static_cast<float_precision>(binomial_coefficient<K>(order, j));

		// Compute (n+j+1)^{k-1}/(n+k+1)^{k-1}
		C_njk  = pow(beta_in_use + static_cast<float_precision>(n + j     + static_cast<K>(1)), static_cast<float_precision>(order - static_cast<K>(1)));
		C_njk /= pow(beta_in_use + static_cast<float_precision>(n + order + static_cast<K>(1)), static_cast<float_precision>(order - static_cast<K>(1)));

		// Compute 1/R_{n+j} where R_{n+j} is the remainder estimate
		rest*= remainder->operator()(
            n + j,
            n + j,
            data.an,
            (remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<float_precision>(1))
        );

		rest *= C_njk;

		denominator += rest;
		numerator += rest * data.Sn.at(n + j);
	}

	numerator /= denominator;

	return numerator;
}

template<UnsignedIntLike K>
inline complex_precision<float_precision> levin_algorithm<complex_precision<float_precision>, K>::calc_result_rec(
	const K n, 
    const K order, 
    const series_result<complex_precision<float_precision>>& data
) const{

	using std::isfinite;
	using std::pow;

	const size_t precision = std::max(
		std::max(data.Sn[0].real().precision(),data.Sn[0].imag().precision()), 
		std::max(data.an[0].real().precision(),data.an[0].imag().precision())
	);

	// For theory, see: Sidi (1979), Section 3 - Recursive implementation using E-algorithm
	// Initialize arrays for recursive computation
	std::vector<complex_precision<float_precision>>   Num(
		order + static_cast<K>(1), 
		complex_precision<float_precision>(
			float_precision(0, precision),
			float_precision(0, precision)
		)
	);
	std::vector<complex_precision<float_precision>> Denom(
		order + static_cast<K>(1), 
		complex_precision<float_precision>(
			float_precision(0, precision),
			float_precision(0, precision)
		)
	);

	// Initialize base values: E_0^{(n)} = S_n, g_0^{(n)} = 1/R_n
	for (K i = static_cast<K>(0); i < order+static_cast<K>(1); ++i) {
		Denom[i] = remainder->operator()(
            n + i,
            n + i,
            data.an,
            complex_precision<float_precision>(remainder_type_in_use == remainder_type::u_type ? beta_in_use : static_cast<float_precision>(1))
        );

		Num[i] = data.Sn.at(n+i) * Denom[i];
	}

	// Recursive computation using the E-algorithm scheme
	float_precision scale = float_precision(0, precision);
	for (K i = static_cast<K>(1); i <= order; ++i)
		for (K j = static_cast<K>(0); j <= order - i; ++j) {


			// For theory, see: Brezinski's E-algorithm recurrence
			// E_k^{(n)} = E_{k-1}^{(n)} - g_{k-1,k}^{(n)} * ΔE_{k-1}^{(n)} / Δg_{k-1,k}^{(n)}
			scale = (beta_in_use + float_precision(n + j));
			scale*= pow(float_precision(1) - static_cast<float_precision>(1) / (beta_in_use + float_precision(n + j + i + 1)), float_precision(i));
			scale/= (beta_in_use + float_precision(n + j + i));

			Denom[j] = fma(complex_precision<float_precision>(-scale),Denom[j],Denom[j+static_cast<K>(1)]);
              Num[j] = fma(complex_precision<float_precision>(-scale),  Num[j],  Num[j+static_cast<K>(1)]);
		}

	Num[0] /= Denom[0];

	return Num[0];
}

template <UnsignedIntLike K>
complex_precision<float_precision> levin_algorithm<complex_precision<float_precision>, K>::operator()(
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
        throw std::out_of_range("The Sn or an smaller then required for L_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;

    const complex_precision<float_precision> result = (use_recurrent_formula ? calc_result_rec(n,order, data) : calc_result(n, order, data));

    if(!isfinite(result.real()) || !isfinite(result.imag())){
        throw std::overflow_error("division by zero");
    }

    return result;
}
#endif
#endif
