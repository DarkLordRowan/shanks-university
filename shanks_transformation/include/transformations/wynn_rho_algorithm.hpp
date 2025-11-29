/**
 * @file wynn_rho_algorithm.hpp
 * @brief This file contains the declaration of the Wynn's Rho Algorithm class.
 * @authors Yurov P.I. Bezzaborov A.A.
 *
 * For theory, see:
 * Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 * Wynn, P. (1966). Upon a second-order continued-fraction transformation for accelerating convergence.
 * Osada, N. (1993). Acceleration Methods for Slowly Convergent Sequences and Their Applications.
 */

#pragma once

#include "series_acceleration.hpp"
#include "wynn_numerators.hpp"
#include <memory>
#include <type_traits>

 /**
  * @brief Wynn's Rho Algorithm class template for sequence acceleration.
  *
  * This class implements Wynn's rho algorithm, which is particularly effective
  * for accelerating logarithmically convergent sequences. The algorithm uses
  * continued fraction representations and reciprocal differences for convergence
  * acceleration.
  *
  * References:
  * - Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
  * - Wynn, P. (1966). Upon a second-order continued-fraction transformation for accelerating convergence.
  * - Osada, N. (1993). Acceleration Methods for Slowly Convergent Sequences and Their Applications.
  *
  * @tparam T Floating-point type for series elements (must satisfy Accepted)
  *           Represents numerical precision (float, double, long double)
  *           Used for all mathematical computations and storage
  * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
  *           Used for counting terms, indexing operations, and order specification
  *           Valid values: K >= 0, typically size_t or unsigned int
  * @tparam series_templ Type of series object to accelerate. Must provide:
  *           - T operator()(K n) const: returns the n-th series term a_n
  *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
  */
template <AcceptedLike T, UnsignedIntLike K>
class wynn_rho_algorithm final : public series_acceleration<T, K>
{
protected:

	using Kostil = std::conditional_t<isFloatLike<T>::value, TypeWrapper<T>, T>;
	using param_type = typename Kostil::value_type;

	std::unique_ptr<const numerator_base<T, K>> numerator;			  /**< Numerator computation strategy */
	param_type gamma_in_use;													  /**< Gamma parameter for generalized rho transformation */
	param_type rho_in_use;													  /**< Rho parameter for gamma-rho variant */
	numerator_type numerator_type_in_use = numerator_type::rho_type;  /**< numerator type in use needed for calculating required size */


public:

	/**
	 * @brief Parameterized constructor to initialize the Rho Wynn Algorithm.
	 *
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 * @param variant Type of numerator computation strategy
	 *        Determines the specific variant of Wynn's rho algorithm:
	 *        - rho_type: Standard Wynn rho algorithm
	 *        - generalized_type: Generalized rho transformation
	 *        - gamma_rho_type: Gamma-rho variant with additional parameters
	 * @param gamma_ Gamma parameter value (default: 1)
	 *        Valid values: T > 0, typically 1 or 2
	 *        Controls the transformation behavior in generalized variants
	 * @param RHO_ Rho parameter value (default: 0)
	 *        Valid values: T >= 0, typically 0 or 1
	 *        Additional parameter for gamma-rho variant
	 */


	// Стал:
	explicit wynn_rho_algorithm(
		numerator_type numerator_type_to_use = numerator_type::rho_type,
		const param_type& gamma_to_use = static_cast<param_type>(-1), // Передача по константной ссылке
		const param_type& rho_to_use   = static_cast<param_type>(1)  // Передача по константной ссылке
	) : series_acceleration<T, K>() {
		update_gamma(gamma_to_use);
		update_rho(rho_to_use);
		update_numerator(numerator_type_to_use);
	};

	//Default destructor is sufficient since unique_ptr handles deletion

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
	 * @param n The starting index for partial sums
	 *        Valid values: n >= 0, determines the starting point of transformation
	 * @param order The order of transformation
	 *        Valid values: order >= 0 and must be even (order % 2 == 0)
	 *        Higher orders use more terms but may provide better acceleration
	 * @return The accelerated partial sum after rho transformation
	 * @throws std::domain_error if order is odd
	 * @throws std::overflow_error if division by zero occurs
	 */
	T operator()(
		const K n, 
        const K order, 
        const series_result<T>& data
	) const override;

	
	void update_gamma(const param_type& new_gamma) { gamma_in_use = new_gamma;}
	void update_rho(const param_type& new_rho) {rho_in_use = new_rho; }

	void update_numerator(const numerator_type numerator_type_to_use){

		numerator_type_in_use = numerator_type_to_use;

		switch(numerator_type_to_use){
        	case numerator_type::rho_type 			: { numerator.reset(new rho_transform<T, K>()	   ); break; }
        	case numerator_type::generalized_type 	: { numerator.reset(new generilized_transform<T, K>()	   ); break; }
        	case numerator_type::gamma_rho_type 	: { numerator.reset(new gamma_rho_transform<T, K>()	   ); break; }
        	default:{
				numerator_type_in_use = numerator_type::rho_type;
        	    numerator.reset(new rho_transform<T, K>()); // Default to u-variant
			}
		}
	}

	std::string get_name() override {

		using std::to_string;

		series_acceleration<T, K>::acceleration_name = "wynn rho algorithm ";
		switch(numerator_type_in_use){
			case numerator_type::rho_type 			: { series_acceleration<T, K>::acceleration_name += "with rho-numerator "; break; }
			case numerator_type::generalized_type 	: { series_acceleration<T, K>::acceleration_name += "with generalized-numerator "; break; }
			case numerator_type::gamma_rho_type 	: { series_acceleration<T, K>::acceleration_name += "with v-numerator "; break; }
		}
		series_acceleration<T, K>::acceleration_name += ", gamma = " + to_string(gamma_in_use);
		series_acceleration<T, K>::acceleration_name += ", rho = " + to_string(rho_in_use);

		return series_acceleration<T, K>::acceleration_name;
	}

};

template <AcceptedLike T, UnsignedIntLike K>
inline T wynn_rho_algorithm<T, K>::operator()(
	const K n, 
    const K order, 
    const series_result<T>& data
) const {

	const K required_size = n + order + static_cast<K>(1) + order * static_cast<K>(numerator_type_in_use == numerator_type::rho_type);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for wynn_rho_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}
    // For theory, see: Brezinski (1977), Chapter 4, Eq. (4.10)
    // Base cases: return partial sum for n=0 or order=0
    if (order == static_cast<K>(0)){
    	return data.Sn.at(n);
	}

	using std::isfinite;

	const K base_size = order + static_cast<K>(1);

    std::vector<T> rho_odd(base_size, static_cast<T>(0)); // vector for theta_(2n + 1)
    std::vector<T> rho_even(base_size, static_cast<T>(0)); //vector for theta_(2n), in the beginning it is theta_(-1) which is zero for all i
	 
	T delta; //temporary varaible
	delta = static_cast<T>(0);

	#ifdef INC_FPRECISION
	if constexpr (is_precisable<T>::value){
		const size_t precision = std::max(utils::get_precision(data.Sn[0]), utils::get_precision(data.an[0]));
		utils::set_vec_precision<T>(rho_odd, precision);
		utils::set_vec_precision<T>(rho_even, precision);
	}
	#endif

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

    if(!isfinite(rho_even[0])){
        throw std::overflow_error("division by zero");
    }

    return rho_even[0];
}

#ifdef INC_COMPLEXPRECISION

template <FloatLike T, UnsignedIntLike K>
class wynn_rho_algorithm<complex_precision<T>, K> final : public series_acceleration<complex_precision<T>, K>
{
protected:

	std::unique_ptr<const numerator_base<complex_precision<T>, K>> numerator;	/**< Numerator computation strategy */
	T gamma_in_use;																	/**< Gamma parameter for generalized rho transformation */
	T rho_in_use;																		/**< Rho parameter for gamma-rho variant */
	numerator_type numerator_type_in_use = numerator_type::rho_type;			/**< numerator type in use needed for calculating required size */

public:

	/**
	 * @brief Constructor to initialize the Rho Wynn Algorithm for T - floating point.
	 *
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 * @param variant Type of numerator computation strategy
	 *        Determines the specific variant of Wynn's rho algorithm:
	 *        - rho_type: Standard Wynn rho algorithm
	 *        - generalized_type: Generalized rho transformation
	 *        - gamma_rho_type: Gamma-rho variant with additional parameters
	 * @param gamma_ Gamma parameter value (default: 1)
	 *        Valid values: T > 0, typically 1 or 2
	 *        Controls the transformation behavior in generalized variants
	 * @param RHO_ Rho parameter value (default: 0)
	 *        Valid values: T >= 0, typically 0 or 1
	 *        Additional parameter for gamma-rho variant
	 */


	// Стал:
	explicit wynn_rho_algorithm(
		numerator_type numerator_type_to_use = numerator_type::rho_type,
		const T& gamma_to_use = static_cast<T>(-1), // Передача по константной ссылке
		const T& rho_to_use   = static_cast<T>(1)  // Передача по константной ссылке
	) : series_acceleration<complex_precision<T>, K>() {
		update_gamma(gamma_to_use);
		update_rho(rho_to_use);
		update_numerator(numerator_type_to_use);
	};

	//Default destructor is sufficient since unique_ptr handles deletion

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
	 * @param n The starting index for partial sums
	 *        Valid values: n >= 0, determines the starting point of transformation
	 * @param order The order of transformation
	 *        Valid values: order >= 0 and must be even (order % 2 == 0)
	 *        Higher orders use more terms but may provide better acceleration
	 * @return The accelerated partial sum after rho transformation
	 * @throws std::domain_error if order is odd
	 * @throws std::overflow_error if division by zero occurs
	 */
	complex_precision<T> operator()(
		const K n, 
        const K order, 
        const series_result<complex_precision<T>>& data
	) const override;

	void update_gamma(const T& new_gamma) { gamma_in_use = new_gamma;}
	void update_rho(const T& new_rho) {rho_in_use = new_rho; }

	void update_numerator(const numerator_type numerator_type_to_use){

		numerator_type_in_use = numerator_type_to_use;

		switch(numerator_type_to_use){
        	case numerator_type::rho_type 			: { numerator.reset(new rho_transform<complex_precision<T>, K>()	   ); break; }
        	case numerator_type::generalized_type 	: { numerator.reset(new generilized_transform<complex_precision<T>, K>()	   ); break; }
        	case numerator_type::gamma_rho_type 	: { numerator.reset(new gamma_rho_transform<complex_precision<T>, K>()	   ); break; }
        	default:{
				numerator_type_in_use = numerator_type::rho_type;
        	    numerator.reset(new rho_transform<complex_precision<T>, K>()); // Default to u-variant
			}
		}
	}

	std::string get_name() override {

		using std::to_string;

		series_acceleration<complex_precision<T>, K>::acceleration_name = "wynn rho algorithm ";
		switch(numerator_type_in_use){
			case numerator_type::rho_type 			: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with rho-numerator "; break; }
			case numerator_type::generalized_type 	: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with generalized-numerator "; break; }
			case numerator_type::gamma_rho_type 	: { series_acceleration<complex_precision<T>, K>::acceleration_name += "with v-numerator "; break; }
		}
		series_acceleration<complex_precision<T>, K>::acceleration_name += ", gamma = " + to_string(gamma_in_use);
		series_acceleration<complex_precision<T>, K>::acceleration_name += ", rho = " + to_string(rho_in_use);

		return series_acceleration<complex_precision<T>, K>::acceleration_name;
	}

};

template <FloatLike T, UnsignedIntLike K>
inline complex_precision<T> wynn_rho_algorithm<complex_precision<T>, K>::operator()(
	const K n, 
    const K order, 
    const series_result<complex_precision<T>>& data
) const { 

	const K required_size = n + order + static_cast<K>(1) + order * static_cast<K>(numerator_type_in_use == numerator_type::rho_type);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for wynn_rho_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}
    // For theory, see: Brezinski (1977), Chapter 4, Eq. (4.10)
    // Base cases: return partial sum for n=0 or order=0
    if (order == static_cast<K>(0)){
    	return data.Sn.at(n);
	}

	const K base_size = order + static_cast<K>(1);

	using std::isfinite;

	std::vector<complex_precision<T>> rho_odd; // vector for theta_(2n + 1)
	std::vector<complex_precision<T>> rho_even; //vector for theta_(2n), in the beginning it is theta_(-1) which is zero for all i
	complex_precision<T> delta; //temporary varaible
	delta = static_cast<complex_precision<T>>(0);

	if constexpr(std::is_floating_point<T>::value){
		rho_even = std::vector<complex_precision<T>>(base_size, static_cast<complex_precision<T>>(0));
		rho_odd = std::vector<complex_precision<T>>(base_size, static_cast<complex_precision<T>>(0));
	}
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value){
		const size_t precision = std::max(
            std::max(data.Sn[0].real().precision(),data.Sn[0].imag().precision()),
            std::max(data.an[0].real().precision(),data.an[0].imag().precision())
        );
		rho_even = std::vector<complex_precision<T>>(base_size, complex_precision<float_precision>(float_precision(0, precision), float_precision(0, precision)));
		rho_odd  = std::vector<complex_precision<T>>(base_size, complex_precision<float_precision>(float_precision(0, precision), float_precision(0, precision)));
		delta.ref_real()->precision(precision); delta.ref_imag()->precision(precision);
	}
	#endif


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
				complex_precision<T>(gamma_in_use), 
				complex_precision<T>(rho_in_use)
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
				complex_precision<T>(gamma_in_use), 
				complex_precision<T>(rho_in_use)
			) / delta;

        }

    }

    if(!isfinite(rho_even[0].real()) || !isfinite(rho_even[0].imag())){
        throw std::overflow_error("division by zero");
    }

    return rho_even[0];
}

#endif