/**
 * @file series_acceleration.hpp
 * @brief This file contains the declaration of the base class series_acceleration and it's derivatives
 */

#pragma once


#include <iostream>   // Include the iostream library for I/O functionalities
#include <math.h>     // Include the math library for mathematical functions
#include <memory>	  // Include for unique ptr

#include "series.h"   //Include for correct work of remainders and numerators types

 /**
  * @brief Enum of transformation IDs
  * @authors Bolshakov M.P.
  * @edited by Kreynin R.G.
*/
enum transformation_id_t {
			   null_transformation_id,
			 shanks_transformation_id,
          theta_modified_algorithm_id,
          	weniger_transformation_id,
		   rho_wynn_transformation_id,
	brezinski_theta_transformation_id,
				 epsilon_algorithm_id,
			   epsilon_algorithm_2_id,
               epsilon_algorithm_3_id,
        epsilon_modified_algorithm_id,
           chang_epsilon_algorithm_id,
    epsilon_aitken_theta_algorithm_id,
				       L_algorithm_id,
                       S_algorithm_id,
					   D_algorithm_id,
                       M_algorithm_id,
					   W_algorithm_id,
			  richardson_algorithm_id,
		       Ford_Sidi_algorithm_id,
		   Ford_Sidi_algorithm_two_id,
		 Ford_Sidi_algorithm_three_id,
};

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                   SERIES ACCELERATION BASE CLASS                                                       //
//                                                                 BEGIN                                                                  //
//----------------------------------------------------------------------------------------------------------------------------------------//

/**
 * @brief Base class series_acceleration
 * @authors Bolshakov M.P.
 * It is not used on it's own, but it is inherited by shanks_transformation and epsilon_algorithm to implement the corresponding methods.
 * The class implementation provides everything needed for construction of an arbitrary series up to n terms and printing out the partial sum,
 * the partial sum after transformation is used, and the difference between the latter and the former.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class series_acceleration
{
public:
	/**
   * @brief Parameterized constructor to initialize the Transformation.
   * @authors Bolshakov M.P.
   * @param series The series class object to be accelerated
   */
	series_acceleration(const series_templ& series);

	/**
   * @brief Method for printing out the info about the object of this class
   * @authors Bolshakov M.P.
   */
	constexpr void print_info() const;

	/**
   * @brief Virtual operator() that returns the partial sum after transformation of the series
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms
   * @param order The order of the transformation
   * @return The transformed partial sum
   */
	virtual T operator()(const K n, const K order) const = 0;

protected:
	/**
   * @brief Series whose convergence is being accelerated
   * @authors Bolshakov M.P.
   */
	series_templ series;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
series_acceleration<T, K, series_templ>::series_acceleration(const series_templ& series) : series(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
constexpr void series_acceleration<T, K, series_templ>::print_info() const
{
	std::cout << "transformation: " << typeid(*this).name() << '\n';
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                   SERIES ACCELERATION BASE CLASS                                                       //
//                                                                  END                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                           REMAINDERS FOR LEVIN-TYPE                                                                    //
//                                                   BEGIN                                                                                //
//----------------------------------------------------------------------------------------------------------------------------------------//

/**
  * @brief Enum for remainders types to use in Levin-type transformations
*/
enum remainder_type{
         u_variant,
         t_variant,
         v_variant,
    t_wave_variant,
    v_wave_variant,
};

/**
* @brief Abstract class for remainders
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class transform_base{
public:

    /**
    * @brief Virtual operator() function for computing remainder
    * @param n The partial sum number (S_n) from which the calculations is done
    * @param order The order of transformation.
    * @param series The series from where to grab terms for remainders
    * @param scale The value to multiple (needed for u variant of S algo for now)
    * @return remainder of necessary type
    */
    virtual T operator() (const K n, const K order, const series_base<T,K>* series, T scale = static_cast<T>(1)) const = 0;
};

/**
* @brief Class for u variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class u_transform : public transform_base<T, K> {
    /**
    * @brief Operator() function for computing u type remainders, such as, w_n = n*a_n (for S algo with beta: w_n = (beta + n) * a_n)
    * @param n The partial sum number (S_n) from which the calculations is done
    * @param order The order of transformation.
    * @param series The series from where to grab terms for remainders
    * @param scale The value to multiple (needed for u variant of S algo for now)
    * @return u type remainder
    */
    T operator() (const K n, const K order, const series_base<T,K>* series, T scale = static_cast<T>(1)) const override;
};

template<std::floating_point T, std::unsigned_integral K>
T u_transform<T, K>::operator()(const K n, const K order, const series_base<T,K>* series, T scale) const {

    using std::isfinite;

    const T result = static_cast<T>(1) / (scale * series->operator()(n+order));

    if (!isfinite(result)) throw std::overflow_error("division by zero");

    return result;
 }

/**
* @brief Class for t variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class t_transform : public transform_base<T, K> {
    /**
    * @brief Operator() function for computing t type remainders, such as, w_n = a_n
    * @param n The partial sum number (S_n) from which the calculations is done
    * @param order The order of transformation.
    * @param series The series from where to grab terms for remainders
    * @param scale not neccesary
    * @return t type remainder
    */
    T operator() (const K n, const K order, const series_base<T,K>* series, T scale = static_cast<T>(1)) const override;
};

template<std::floating_point T, std::unsigned_integral K>
T t_transform<T, K>::operator()(const K n, const K order, const series_base<T,K>* series, T scale) const {

    using std::isfinite;

    const T result = static_cast<T>(1) / series->operator()(n+order);
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}

/**
* @brief Class for t-wave variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class t_wave_transform : public transform_base<T, K>  {
    /**
    * @brief Operator() function for computing t-wave type remainders, such as, w_n = a_(n+1)
    * @param n The partial sum number (S_n) from which the calculations is done
    * @param order The order of transformation.
    * @param series The series from where to grab terms for remainders
    * @param scale not neccesary
    * @return t-wave type remainder
    */
    T operator() (const K n, const K order, const series_base<T,K>* series, T scale = static_cast<T>(1)) const override;
};

template<std::floating_point T, std::unsigned_integral K>
T t_wave_transform<T,K>::operator()(const K n, const K order, const series_base<T, K>* series, T scale ) const {

    using std::isfinite;

	const T result = static_cast<T>(1) / series->operator()(n + order + 1);

	if (!isfinite(result)) throw std::overflow_error("division by zero");
	return result;
}

/**
* @brief Class for v variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class v_transform : public transform_base<T, K> {
    /**
    * @brief Operator() function for computing v type remainders, such as, w_n = a_n*a_(n+1)/(a_(n+1)-a_n)
    * @param n The partial sum number (S_n) from which the calculations is done
    * @param order The order of transformation.
    * @param series The series from where to grab terms for remainders
    * @param scale not neccesary
    * @return v type remainder
    */
    T operator() (const K n, const K order, const series_base<T,K>* series, T scale = static_cast<T>(1)) const override;
};

template<std::floating_point T, std::unsigned_integral K>
T v_transform<T,K>::operator()(const K n, const K order, const series_base<T,K>* series, T scale) const {

    using std::isfinite;

    const T a1 = series->operator()(n+order), a2  = series->operator()(n+order+1);
    const T result = (a2-a1) / (a1 * a2);
	if (!isfinite(result)) throw std::overflow_error("division by zero");
	return result;
}

/**
* @brief Class for v-wave variant of remainder
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class v_wave_transform : public transform_base<T, K> {
    /**
    * @brief Operator() function for computing v-wave type remainders, such as, w_n = a_n*a_(n+1)/(a_n-a_(n+1))
    * @param n The partial sum number (S_n) from which the calculations is done
    * @param order The order of transformation.
    * @param series The series from where to grab terms for remainders
    * @param scale not neccesary
    * @return v-wave type remainder
    */
    T operator() (const K n, const K order, const series_base<T,K>* series, T scale = T(1)) const override;
};

template<std::floating_point T, std::unsigned_integral K>
T v_wave_transform<T,K>::operator()(const K n, const K order, const series_base<T,K>* series, T scale) const  {

    using std::isfinite;
    
    const T a1 = series->operator()(n+order), a2 = series->operator()(n+order+1);
    const T result = (a1 - a2) / (a1 * a2);
	if (!isfinite(result)) throw std::overflow_error("division by zero");
	return result;
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                           REMAINDERS FOR LEVIN-TYPE                                                                    //
//                                                       END                                                                              //
//----------------------------------------------------------------------------------------------------------------------------------------//





//----------------------------------------------------------------------------------------------------------------------------------------//
//                                           NUMERATORS FOR RHO-WYNN TRANSFORMATION                                                       //
//                                                       BEGIN                                                                            //
//----------------------------------------------------------------------------------------------------------------------------------------//

/**
  * @brief Enum for remainders types to use in Levin-type transformations
*/
enum numerator_type{
           rho_variant,
   generalized_variant,
     gamma_rho_variant,
};

/**
 * @brief Abstract class for numerator
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 */

template<std::floating_point T, std::unsigned_integral K>
class numerator_base {
public:

	/**
     * @brief Virtual operator() function for computing numerator
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @param series The series from where to grab terms for numerator
     * @param gamma const for transformation	(	rho(gamma)		)
     * @param rho const for transformation	(	rho(gamma,rho)	)
     * @return The special numerator for transformation
     */
	virtual T operator()(
		const K n, 
		const K order, 
		const series_base<T, K>* series, 
		const T gamma = static_cast<T>(1), 
		const T rho = static_cast<T>(0)
	) const = 0;

};

/**
* @brief Class for rho variant of numerator
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class rho_transform : public numerator_base<T, K> {
public:

	/**
   * @brief Operator() function for computing rho numerator
   *	x_n+order - x_n
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @param series The series from where to grab terms for numerator
   * @param gamma const for transformation	(	rho(gamma)		)
   * @param rho const for transformation	(	rho(gamma,rho)	)
   * @return The special numerator for transformation
   */

	T operator()(
		const K n, 
		const K order, 
		const series_base<T, K>* series,
		const T gamma = static_cast<T>(1), 
		const T rho = static_cast<T>(0)
	) const override {
		return (series->operator()(n + order) - series->operator()(n));
	}
};


/**
* @brief Class for generilized variant of numerator
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<typename T, typename K>
class generilized_transform : public numerator_base<T, K> {
public:

	/**
   * @brief Operator() function for computing generilized numerator
   *	order-gamma-1
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @param series The series from where to grab terms for numerator
   * @param gamma const for transformation	(	rho(gamma)		)
   * @param rho const for transformation	(	rho(gamma,rho)	)
   * @return The special numerator for transformation
   */

	T operator()(
		const K n, 
		const K order, 
		const series_base<T, K>* series, 
		const T gamma = static_cast<T>(1), 
		const T rho = static_cast<T>(0)
	) const override {
		return static_cast<T>(order) - gamma - static_cast<T>(1);
	}
};


/**
* @brief Class for gamma-rho variant of numerator
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<std::floating_point T, std::unsigned_integral K>
class gamma_rho_transform : public numerator_base<T, K> {
public:

	/**
   * @brief Operator() function for computing gamma-rho numerator
   *	C_2j	 = -gamma + j/rho
   *	C_2j+1	 = -gamma + j/rho + 1
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @param series The series from where to grab terms for numerator
   * @param gamma const for transformation	(	rho(gamma)		)
   * @param rho const for transformation	(	rho(gamma,rho)	)
   * @return The special numerator for transformation
   */

	T operator()(
		const K n, 
		const K order, 
		const series_base<T, K>* series, 
		const T gamma = static_cast<T>(1), 
		const T rho = static_cast<T>(0)
	) const override {

		// insight: order % 2 is the same order & 1
		// if order is even:
		// order % 2 = 0
		// order & 1 = 0
		// else order is odd:
		// order % 2 = 1
		// order & 1 = 1

		return (-gamma + static_cast<T>(order / 2) / rho + static_cast<T>(order & 1));
	}
};

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                           NUMERATORS FOR RHO-WYNN TRANSFORMATION                                                       //
//                                                          END                                                                           //
//----------------------------------------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                           LEVIN L TRANSFORMATION                                                       //
//                                                                 BEGIN                                                                  //
//----------------------------------------------------------------------------------------------------------------------------------------//

/**
  * @brief Levin Algorithm class template.
  * @authors  Kreinin R.G., Trudolyubov N.A.
  * @tparam T Element type of the series
  * @tparam K Integer index type
  * @tparam series_templ Series type to accelerate
*/
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class levin_L_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	const T beta;
	bool useRecFormulas = false;

	inline T calc_result(K n, K order) const;

	T operator()(
		const K n_time, 
		const K k_time, 
		const T b, 
		const bool ND
	) const;

	inline T calc_result_rec(K n, K order) const;

public:
	/**
	 * @brief Parameterized constructor to initialize the Levin Algorithm.
	 * @param series The series class object to be accelerated
	*/

	explicit levin_L_algorithm(
		const series_templ& series, 
		const bool useRecFormulas = false, 
		const T beta = static_cast<T>(-1.5)
	);

	/**
	 * @brief Fast impimentation of Levin algorithm.
	 * Computes the partial sum after the transformation using the Levin Algorithm.
  	 * For more information, see 3.9.13 in [https://dlmf.nist.gov/3.9]
	 * @param n The number of terms in the partial sum.
	 * @param order The order of transformation.
	 * @return The partial sum after the transformation.
	*/

	T operator()(const K n, const K order) const override;
};

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
levin_L_algorithm<T, K,series_templ>::levin_L_algorithm(
	const series_templ& series, 
	const bool useRecFormulas, 
	const T beta
	) :
	series_acceleration<T, K, series_templ>(series),
	useRecFormulas(useRecFormulas),
	beta(beta)
	{//TODO: нужно ли проверять бету на допустимость?
		}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T levin_L_algorithm<T, K,series_templ>::calc_result(K n, K order) const{

	using std::pow;
	using std::isfinite;

	T numerator = static_cast<T>(0), denominator = static_cast<T>(0);
	T C_njk, S_nj, g_n, rest;

	for (K j = 0; j <= order; ++j) { //Standart Levin algo procedure

		rest  = this->series->minus_one_raised_to_power_n(j);
		rest *= this->series->binomial_coefficient(static_cast<T>(order), j);

		C_njk  = static_cast<T>(pow(n + j     + 1, order - 1));
		C_njk /= static_cast<T>(pow(n + order + 1, order - 1));

		S_nj = this->series->S_n(n + j);

		g_n = static_cast<T>(1) / (this->series->operator()(n + j));

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

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T levin_L_algorithm<T, K, series_templ>::operator()(const K n_time, const K k_time, const T b, const bool ND) const {

	using std::isfinite;

    T w_n = static_cast<T>(1-2*(n_time % 2)) * this->series->fact(n_time);
    T R_0 = (ND == static_cast<T>(0) ? this->series->S_n(n_time) : static_cast<T>(1)); R_0 /= w_n;

    if (k_time == static_cast<K>(0)) return R_0;

    const K a1 = k_time - static_cast<K>(1);
    const T a2 = b + static_cast<T>(n_time);
    const T a3 = static_cast<T>(a1) + a2;
    const T res = static_cast<T>(
        fma(
            -a2 * (*this)(n_time, a1, b, ND),
            pow(a3, a1 - static_cast<T>(1)) / pow(a3 + static_cast<T>(1), a1),
            (*this)(
                n_time + static_cast<K>(1), 
                a1, 
                b, 
                ND
            )
        )
    );

    if (!isfinite(res)) throw std::overflow_error("division by zero");

     return res;
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T levin_L_algorithm<T, K,series_templ>::calc_result_rec(K n, K order) const{

	using std::isfinite;

    const T result = (*this)(n, order, beta, 0) / (*this)(n, order, beta, 1);

    if (!isfinite(result)) throw std::overflow_error("division by zero");

    return result;
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T levin_L_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	using std::isfinite;

	if (n == static_cast<K>(0)) 
        throw std::domain_error("n = 0 in the input"); //TODO: диагнастичекский ретурн, что делать
	if (order == static_cast<K>(0)) return this->series->S_n(n);

    const T result = (useRecFormulas ? calc_result_rec(n,order) : calc_result(n, order));
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                           LEVIN L TRANSFORMATION                                                       //
//                                                                  END                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//



//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                       DRUMMONDS D TRANSFORMATION                                                       //
//                                                                 BEGIN                                                                  //
//----------------------------------------------------------------------------------------------------------------------------------------//

/**
 * @brief D_transformation class template.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 * @param remainder_func - remainder to use in formula
 * @param useRecFormulas - use reccurence relation, the calculation will be done using arrays
 * @param variant        - type of remainder to use
*/

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class drummonds_D_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

    std::unique_ptr<const transform_base<T, K>> remainder_func;
    bool useRecFormulas = false;
    remainder_type variant = remainder_type::u_variant;

	/**
	* @brief Function to calculate D-tranformation directly by formula. For more information see p. 70 9.5-4 [https://arxiv.org/pdf/math/0306302.pdf]
	* @param n The partial sum number (S_n) from which the calculations will be done
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/

	inline T calc_result(const K n, const K order) const;

	/**
	* @brief Function to calculate D-tranformation using reccurence relation. For more information see p. 70 9.5-5 [https://arxiv.org/pdf/math/0306302.pdf]
	* @param n The partial sum number (S_n) from which the calculations will be done
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/

	inline T calc_result_rec(const K n, const K order) const;

public:

	/**
	* @brief Parameterized explicit constructor to initialize the Drummond's D Algorithm.
	* @param series The series class object to be accelerated
	* @param variant Type of remainder to use
	* @param useRecFormulas use reccurence or straightforward formula 
	*/

	explicit drummonds_D_algorithm(
		const series_templ& series, 
		const remainder_type variant = remainder_type::u_variant, 
		bool useRecFormulas = false
	);

	/**
    * @brief D-transformation.
    * Computes the partial sum after the D-transformation
    * @param n The partial sum number (S_n) from which the calculations will be done
    * @param order The order of transformation.
    * @return The partial sum after the transformation.
    */

    T operator()(const K n, const K order) const override;

};

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T drummonds_D_algorithm<T,K,series_templ>::calc_result(const K n, const K order) const {

    using std::isfinite;

	T numerator = static_cast<T>(0), denominator = static_cast<T>(0);
	T rest;

	for (K j = 0; j <= n; ++j) {

		rest  = this->series->minus_one_raised_to_power_n(j);
		rest *= this->series->binomial_coefficient(static_cast<T>(n), j);
		rest *= remainder_func->operator()(n,j, this->series);

		numerator   += rest * this->series->S_n(n+j);
		denominator += rest;
	}

	numerator /= denominator;

	if (!isfinite(numerator))
		throw std::overflow_error("division by zero");
	return numerator;
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T drummonds_D_algorithm<T,K,series_templ>::calc_result_rec(const K n, const K order) const {

    using std::isfinite;

	std::vector<T>   Num(order + 1, static_cast<T>(0));
	std::vector<T> Denom(order + 1, static_cast<T>(0));

    //init the base values
	for (K i = 0; i < order+1; ++i) {
		Denom[i] = remainder_func->operator()(n, i, this->series);
		  Num[i] = this->series->S_n(n+i) * Denom[i];
	}

    //recurrence
	for (K i = 1; i <= order; ++i)
		for (K j = 0; j <= order - i; ++j) {
			Denom[j] = Denom[j+1] - Denom[j];
			  Num[j] =   Num[j+1] -   Num[j];
		}

	Num[0] /= Denom[0];

	if (!isfinite(Num[0]))
		throw std::overflow_error("division by zero");
    
	return Num[0];
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
drummonds_D_algorithm<T,K,series_templ>::drummonds_D_algorithm(
	const series_templ& series, 
	const remainder_type variant, 
	bool useRecFormulas
	) : 
	series_acceleration<T, K, series_templ>(series),
	useRecFormulas(useRecFormulas)
{
    //check variant else default 'u'
    switch(variant){
        case remainder_type::u_variant :
            remainder_func.reset(new u_transform<T, K>());
            break;
        case remainder_type::t_variant :
            remainder_func.reset(new t_transform<T, K>());
            break;
        case remainder_type::v_variant :
            remainder_func.reset(new v_transform<T, K>());
            break;
        case remainder_type::t_wave_variant:
            remainder_func.reset(new t_wave_transform<T, K>());
            break;
        case remainder_type::v_wave_variant:
            remainder_func.reset(new v_wave_transform<T, K>());
            break;
        default:
            remainder_func.reset(new u_transform<T, K>());
            break;
    }
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T drummonds_D_algorithm<T,K,series_templ>::operator()(const K n, const K order) const { 

    using std::isfinite;

    const T result = (useRecFormulas ? calc_result_rec(n,order) : calc_result(n, order));
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                       DRUMMONDS D TRANSFORMATION                                                       //
//                                                                  END                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//



//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                      LEVIN SIDI S TRANSFORMATION                                                       //
//                                                                 BEGIN                                                                  //
//----------------------------------------------------------------------------------------------------------------------------------------//

/**
 * @brief S_transformation class template.
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 * @tparam series_templ is the type of series whose convergence we accelerate
 * @param beta           - nonzero positive parameter, 1 is the standart value, for more information see p. 39 in [https://arxiv.org/pdf/math/0306302.pdf]
 * @param remainder_func - remainder to use in calculations
 * @param useRecFormulas - use reccurence relation, the calculation will be done using arrays
 * @param variant        - type of remainder to use
*/
template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class levin_sidi_S_algorithm : public series_acceleration<T, K, series_templ> {
protected:

    T beta;
    std::unique_ptr<const transform_base<T, K>> remainder_func;
    bool useRecFormulas = false;
    remainder_type variant = remainder_type::u_variant;

    /**
	* @brief Function to calculate S-tranformation directly by formula. For more information see p. 57 8.2-7 [https://arxiv.org/pdf/math/0306302.pdf]
	* @param n The partial sum number (S_n) from which the calculations will be done
	* @param order the order of transformationA
	* @return The partial sum after the transformation.
	*/
    inline T calc_result(K n, K order) const;

    /**
	* @brief Function to calculate S-tranformation using reccurence relation. For more information see p. 57 8.3-5 [https://arxiv.org/pdf/math/0306302.pdf]
	* @param n The partial sum number (S_n) from which the calculations will be done
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/
    inline T calc_result_rec(K n, K order) const;

public:

    /**
	* @brief Parameterized constructor to initialize the Levin-Sidi S-transformation.
	* @param series The series class object to be accelerated
	* @param variant Type of remainder to use
	* @param useRecFormulas use reccurence or straightforward formula 
    * @param parameter value of beta parameter
	*/

    explicit levin_sidi_S_algorithm(
        const series_templ& series, 
        const remainder_type variant = remainder_type::u_variant, 
        bool useRecFormulas = false,  
        T parameter = static_cast<T>(1)
    );

    /**
    * @brief S-transformation.
    * Computes the partial sum after the S-transformation
    * @param n The partial sum number (S_n) from which the calculations will be done
    * @param order The order of transformation.
    * @return The partial sum after the transformation.
    */
    T operator()(const K n, const K order) const override;

};

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T levin_sidi_S_algorithm<T, K,series_templ>::calc_result(K n, K order) const{

    using std::isfinite;

    T numerator = static_cast<T>(0), denominator = static_cast<T>(0);
    T rest;
    T up_pochamer, down_pochamer; 
    for (K j = 0; j <= order; ++j){
        rest  = this->series->minus_one_raised_to_power_n(j);
        rest *= this->series->binomial_coefficient(order, j);

        up_pochamer = down_pochamer = static_cast<T>(1);
        //up_pochamer   (beta + n + j)_(order - 1)     = (beta + n + j)(beta + n + j + 1)...(beta + n + j + order - 2)
        //down_pochamer (beta + n + order)_(order - 1) = (beta + n + order)(beta + n + order + 1)...(beta + n + order + oreder - 2)
        for (K i = 0; i < order - 1; ++i){
            up_pochamer   *= (beta + static_cast<T>(n + j     + i));
            down_pochamer *= (beta + static_cast<T>(n + order + i));
        }

        rest *= (up_pochamer / down_pochamer);
        rest *= remainder_func->operator()(n, 
            j, 
            this->series,
             (variant == remainder_type::u_variant ? beta : T(1))
            );

        numerator   += rest * this->series->S_n(n + j);
        denominator += rest;
    }

    numerator /= denominator;
    
    if (!isfinite(numerator)) throw std::overflow_error("division by zero");
    return numerator;
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T levin_sidi_S_algorithm<T, K,series_templ>::calc_result_rec(K n, K order) const{

    using std::isfinite;

    std::vector<T>   Num(order + 1, static_cast<T>(0));
    std::vector<T> Denom(order + 1, static_cast<T>(0));

    for (K i = 0; i <= order; ++i){

        Denom[i] = remainder_func->operator()(
            n, 
            i, 
            this->series,
            (variant == remainder_type::u_variant ? beta : T(1))
        );

        Num[i] = this->series->S_n(n + i); Num[i] *= Denom[i];
    }
    
    T scale1, scale2;
    for (K i = 1; i <= order; ++i)
        for(K j = 0; j <= order - i; ++j){

            // i ~ k from formula
            // j ~ n from formula
            scale1 = beta + static_cast<T>(i + j);
            scale1*= (scale1 - T(1));
            scale2 = scale1 + static_cast<T>(i);
            scale2*= (scale2 - T(1));

            Denom[j] = fma(-scale1,Denom[j]/scale2,Denom[j+1]);
              Num[j] = fma(-scale1,  Num[j]/scale2,  Num[j+1]);
        }
    
    Num[0] /= Denom[0];
    
    if (!isfinite(Num[0])) throw std::overflow_error("division by zero");
    return Num[0];
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
levin_sidi_S_algorithm<T, K, series_templ>::levin_sidi_S_algorithm(
    const series_templ& series, 
    remainder_type variant, 
    bool useRecFormulas,  
    T parameter
    ) : 
    series_acceleration<T, K, series_templ>(series),
    useRecFormulas(useRecFormulas)
{
    // parameter is "beta" parameter
    // beta must be nonzero positive real number
    // beta = 1 is default
    //check parameter else default
    if (parameter > static_cast<T>(0)) this->beta = parameter;
    else {
        this->beta = static_cast<T>(1);
        //TODO: наверное сообщение по типу usage
    }

    //check variant else default 'u'
    //TODO: тоже самое наверное
    switch(variant){
        case remainder_type::u_variant :
            remainder_func.reset(new u_transform<T, K>());
            break;
        case remainder_type::t_variant :
            remainder_func.reset(new t_transform<T, K>());
            break;
        case remainder_type::v_variant :
            remainder_func.reset(new v_transform<T, K>());
            break;
        case remainder_type::t_wave_variant:
            remainder_func.reset(new t_wave_transform<T, K>());
            break;
        case remainder_type::v_wave_variant:
            remainder_func.reset(new v_wave_transform<T, K>());
            break;
        default:
            remainder_func.reset(new u_transform<T, K>());
    }
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T levin_sidi_S_algorithm<T, K, series_templ>::operator()(const K n, const K order) const { 

    using std::isfinite;

    const T result = (useRecFormulas ? calc_result_rec(n,order) : calc_result(n, order));
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}


//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                      LEVIN SIDI S TRANSFORMATION                                                       //
//                                                                  END                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//




//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                      LEVIN SIDI M TRANSFORMATION                                                       //
//                                                                 BEGIN                                                                  //
//----------------------------------------------------------------------------------------------------------------------------------------//

/**
 * @brief M_transformation class template.
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 * @tparam series_templ is the type of series whose convergence we accelerate
 * @param remainder_func - remainder type
 */

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class levin_sidi_M_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	const T gamma;
	std::unique_ptr<const transform_base<T, K>> remainder_func;

	/**
	 * @brief Default function to calculate M-transformation. Implemented u,t,d and v transformations. For more information see p. 65 9.2-6 [https://arxiv.org/pdf/math/0306302.pdf]
	 * Levin-Sidi or Factorial analog of Levin Transformation is effective for series that belong to b(1)/LIN/FAC and inferior on b(1)/LOG for more information see p. 369 and p.285 [http://servidor.demec.ufpr.br/CFD/bibliografia/MER/Sidi_2003.pdf]
	 * @authors Yurov P.I. Bezzaborov A.A.
	 * @param n The number of terms in the partial sum.
	 * @param order the order of transformation
	 * @param remainder_func functor, whose returning w_n for u,t,d or v transformation
	 * @return The partial sum after the transformation.
	 * We assume that the Pochhammer symbol satisfies (-x)_n = (-1)^n*(x-n+1)_n
	 */

	inline T calculate(const K n, const K order) const;

public:

	/**
	c* @brief Parameterized constructor to initialize the Levin-Sidi M-transformation.
	 * @param series The series class object to be accelerated
	 * @param variant Remainder type
	 * @param gamma_ Parameter //TODO более подробно
	*/

	//levin_sidi_M_algorithm(const series_templ& series, const transform_base<T, K>* func, const T gamma_ = T(10)) : series_acceleration<T, K, series_templ>(series), remainder_func(func), gamma(gamma_)
	explicit levin_sidi_M_algorithm(
		const series_templ& series, 
		const remainder_type variant = remainder_type::u_variant, 
		const T gamma_ = static_cast<T>(10)
	);

	// Default destructor is sufficient since unique_ptr handles deletion

	/**
     * @brief M-transformation.
     * Computes the partial sum after the M-transformation
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */

	T operator()(const K n, const K order) const override;
};

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T levin_sidi_M_algorithm<T, K, series_templ>::calculate(const K n, const K order) const {

    using std::isfinite;

	if (gamma - static_cast<T>(n - 1) <= static_cast<T>(0))
		throw std::domain_error("gamma cannot be lesser than n-1");
	T numerator = static_cast<T>(0), denominator = static_cast<T>(0);

	T rest;

	T up = static_cast<T>(1), down = static_cast<T>(1);
	T binomial_coef = this->series->binomial_coefficient(static_cast<T>(n), 0);
	T S_n = this->series->S_n(order);

	T down_coef = gamma + static_cast<T>(order + 2);
	T   up_coef = down_coef - static_cast<T>(n);
	
	for (K m = 0; m < n - 1; ++m) {
		up   *= (up_coef   + static_cast<T>(m));
		down *= (down_coef + static_cast<T>(m));
	}
	up /= down;
	down_coef = gamma + static_cast<T>(order + 1);
	up_coef   = down_coef - static_cast<T>(n + 1);
	
	for (K j = 0; j <= n; ++j) {
		rest  = this->series->minus_one_raised_to_power_n(j); 
		rest *= binomial_coef * static_cast<T>(n - j);
		rest *= up; 
		rest /= static_cast<T>(j + 1);
		up /= (  up_coef + static_cast<T>(j));
		up *= (down_coef + static_cast<T>(j));
		rest *= remainder_func->operator()(
			order, 
			j, 
			this->series, 
			-gamma-static_cast<T>(n)
		);
		numerator += rest * S_n ;
		denominator += rest;
		S_n += this->series->operator()(order + j + 1);
	}

	numerator /= denominator;

	if (!isfinite(numerator))
		throw std::overflow_error("division by zero");
	return numerator;
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T levin_sidi_M_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {
	return calculate(n, order);
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
levin_sidi_M_algorithm<T, K, series_templ>::levin_sidi_M_algorithm(
	const series_templ& series, 
	const remainder_type variant, 
	const T gamma_
	) :
	series_acceleration<T, K, series_templ>(series),
	gamma(gamma_)
{

	switch(variant){
        case remainder_type::u_variant :
            remainder_func.reset(new u_transform<T, K>());
            break;
        case remainder_type::t_variant :
            remainder_func.reset(new t_transform<T, K>());
            break;
        case remainder_type::v_variant :
            remainder_func.reset(new v_transform<T, K>());
            break;
        case remainder_type::t_wave_variant:
            remainder_func.reset(new t_wave_transform<T, K>());
            break;
        case remainder_type::v_wave_variant:
            remainder_func.reset(new v_wave_transform<T, K>());
            break;
        default:
            remainder_func.reset(new u_transform<T, K>());
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                      LEVIN SIDI M TRANSFORMATION                                                       //
//                                                                  END                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//



//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                          RHO WYNN ALGORITHM                                                            //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

 /**
  * @brief Rho Wynn Algorithm class template.
  * @tparam T The type of the elements in the series
  * @tparam K The type of enumerating integer
  * @tparam series_templ is the type of series whose convergence we accelerate
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class rho_Wynn_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	std::unique_ptr<const numerator_base<T, K>> numerator_func;
	const T gamma;
	const T RHO;

	inline T calculate(const K n, K order) const;

	T recursive_calculate_body(const K n, const K order, T S_n, const K j) const;

public:
	/**
     * @brief Parameterized constructor to initialize the Rho Wynn Algorithm.
     * @param series The series class object to be accelerated
     */
	explicit rho_Wynn_algorithm(
		const series_templ& series, 
		const numerator_type variant = numerator_type::rho_variant, 
		const T gamma_ = T(1), 
		const T RHO_ = T(0)
	);

	//Default destructor is sufficient since unique_ptr handles deletion

	/**
     * @brief Rho Wynn algorithm.
     * Computes the partial sum after the transformation using the Rho Wynn Algorithm.
     * For more information, see
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */
	T operator()(const K n, const K order) const override { return calculate(n, order); }

	/**
	 * @brief Compute transformed partial sum (extended version for arbitrary precision)
	 * @tparam BigK Type for term count (auto-deduced)
	 * @tparam BigOrder Type for order (auto-deduced)
	 * @param n Number of terms (passed by reference for large types)
	 * @param order Transformation order (must be even, passed by reference)
	 * @return The accelerated partial sum
	 * @throws std::domain_error for invalid arguments
	 * @throws std::overflow_error for division by zero
	 */
    //надо посмотреть может годная тема
	//template <typename BigK, typename BigOrder, typename = std::enable_if_t<!std::is_same_v<BigK, K> || !std::is_same_v<BigOrder, K>>> T operator()(const BigK& n, const BigOrder& order) const {
	//	static_assert(std::is_constructible_v<K, BigK>, "Term count type must be convertible to K");
    //
	//	return calculate_impl(static_cast<K>(n), static_cast<int>(order));
	//}
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
rho_Wynn_algorithm<T, K, series_templ>::rho_Wynn_algorithm(
	const series_templ& series, 
	const numerator_type variant,
	const T gamma_, 
	const T RHO_
	) : 
	series_acceleration<T, K, series_templ>(series), 
	gamma(gamma_), 
	RHO(RHO_) 
{
	//TODO: проверить можно ли использовать стандартные значения, указанный в конструктуре, в других вариантах + унифицировать названия
	// есть ли какие то ограничения на кастомные значения?
	// classic (0), gamma (1), gamma-rho (2): 
	// 0 -> gamma = 1, RHO = 0
	// 1 -> gamma = 2, RHO = 0
	// 2 -> gamma = 2, RHO = 1

	switch(variant) {
		case numerator_type::rho_variant :
			numerator_func.reset(new rho_transform<T, K>());
			break;
		case numerator_type::generalized_variant :
			numerator_func.reset(new rho_transform<T, K>());
			break;
		case numerator_type::gamma_rho_variant :
			numerator_func.reset(new rho_transform<T, K>());
			break;
		default:
			numerator_func.reset(new rho_transform<T, K>());
	}
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T rho_Wynn_algorithm<T, K, series_templ>::calculate(const K n, K order) const { //const int order	

	using std::isfinite;

	if (order & 1)// is order odd
		throw std::domain_error("order should be even number");

	if (order == static_cast<K>(0)) return this->series->S_n(n);

	const T S_n = this->series->S_n(n);
	const K order1 = order - static_cast<K>(1);
	const T res = (recursive_calculate_body(n, order1 - 1, S_n, 1) + numerator_func->operator()(n, order, this->series, gamma, RHO)) / 
		(recursive_calculate_body(n, order1, S_n, 1) - recursive_calculate_body(n, order1, S_n, 0));

	if (!isfinite(res))
		throw std::overflow_error("division by zero");
	return res;	
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T rho_Wynn_algorithm<T, K, series_templ>::recursive_calculate_body(const K n, const K order, T S_n, const K j) const {

	using std::isfinite;

	/**
	* S_n - previous sum;
	* j - adjusts n: (n + j);
	*/

	S_n += (j == static_cast<K>(0)) ? static_cast<T>(0) : this->series->operator()(n + j);

	if (order == static_cast<K>(0))
		return S_n;

	if (order == static_cast<K>(-1))
		return static_cast<T>(0);

	const K order1 = order - static_cast<K>(1);
	const K nj = n + j;
	const T res = (recursive_calculate_body(nj, order1 - static_cast<K>(1), S_n, 1) + numerator_func->operator()(nj, order, this->series, gamma, RHO)) /
		(recursive_calculate_body(nj, order1, S_n, 1) - recursive_calculate_body(nj, order1, S_n, 0));

	if (!isfinite(res))
		throw std::overflow_error("division by zero");

	return res;
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                          RHO WYNN ALGORITHM                                                            //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                           WENIGER ALGORITHM                                                            //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class weniger_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	/**
	  * @brief Default function to calculate WT-transformation.
	  * For more information see "Joint use of the Weniger transformation and hyperasymptotics for accurate asymptotic evaluations of a class of saddle-point integrals"
	  * @param k The number of terms in the partial sum.
	  * @param n the order of transformation
	  * @param remainder_func functor, whose returning w_n for t,u or v transformation
  	  * @return The partial sum after the transformation.
	  */

public:
	
	/**
	 * @brief Weniger class template for derivations
	 * @tparam T The type of the elements in the series
	 * @tparam K The type of enumerating integer
	 * @tparam series_templ is the type of series whose convergence we accelerate
	 */

	explicit weniger_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	* @brief Abstract method for the inherited classes below
	* Computes the partial sum after the transformation using the epsilon Weniger Algorithm.
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @return The partial sum after the transformation.
	*/

	T operator()(const K n, const K order) const override;
};

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T weniger_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	using std::isfinite;

	T numerator = static_cast<T>(0), denominator = static_cast<T>(0);

	T a_n, rest;

	T coef = static_cast<T>(1);
	T binomial_coef = this->series->binomial_coefficient(static_cast<T>(n), 0);
	T S_n = this->series->S_n(0);

	for (K m = 0; m < order - 1; ++m) 
		coef *= static_cast<T>(1 + m);
	
	T j1;

	for (K j = 0; j <= order; ++j) {

		j1 = static_cast<T>(j + 1);

		rest  = this->series->minus_one_raised_to_power_n(j);
		rest *= binomial_coef;

		binomial_coef *= static_cast<T>(order - j) / j1;
		rest *= coef;
		coef *= static_cast<T>(j + order);
		coef /= j1;

		a_n = static_cast<T>(1) / this->series->operator()(j1);

		rest *= a_n;

		numerator += rest * S_n;
		denominator += rest;
		S_n += this->series->operator()(j + 1);
		
	}

	numerator /= denominator;

	if (!isfinite(numerator))
		throw std::overflow_error("division by zero");
	
	return numerator;
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                           WENIGER ALGORITHM                                                            //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                        SHANKS TRANSFORMATION                                                           //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

 /**
 * @brief Shanks transformation for non-alternating series class.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class shanks_transform : public series_acceleration<T, K, series_templ>
{
public:

	/**
   * @brief Parameterized constructor to initialize the Shanks transformation for non-alternating series.
   * @authors Bolshakov M.P.
   * @param series The series class object
   */
	explicit shanks_transform(const series_templ& series);

	/**
   * @brief Shanks transformation for non-alternating function.
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @return The partial sum after the transformation.
   */
	T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
shanks_transform<T, K, series_templ>::shanks_transform(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T shanks_transform<T, K, series_templ>::operator()(const K n, const K order) const{

	using std::isfinite;
	using std::fma;

	if (order == static_cast<K>(0)) [[unlikely]] /*it is convenient to assume that transformation of order 0 is no transformation at all*/
		return this->series->S_n(n);

	if (n < order || n == static_cast<K>(0)) [[unlikely]]
		return static_cast<T>(0); // TODO: диагностика

	if (order == static_cast<K>(1)) [[unlikely]] 
	{
		T a_n, a_n_plus_1, tmp;

		a_n = this->series->operator()(n);
		a_n_plus_1 = this->series->operator()(n + 1);
		tmp = -a_n_plus_1 * a_n_plus_1;

		const T result = fma(a_n * a_n_plus_1, (a_n + a_n_plus_1) / (fma(a_n, a_n, tmp) - fma(a_n_plus_1, a_n_plus_1, tmp)), this->series->S_n(n));

		if (!isfinite(result))
			throw std::overflow_error("divison by zero");
		return result;
	}
	//n > order >= 1

	const K n_minus_order = n - order;
	const K n_plus_order = n + order;

	std::vector<T> T_n(n_plus_order, static_cast<T>(0));

	T a_n, a_n_plus_1, tmp;
	a_n = this->series->operator()(n_minus_order);
	a_n_plus_1 = this->series->operator()(n_minus_order + 1);
	tmp = -a_n_plus_1 * a_n_plus_1;

	for (K i = n_minus_order + 1; i <= n_plus_order - 1; ++i) // if we got to this branch then we know that n >= order - see previous branches  int -> K
	{
		a_n = this->series->operator()(i);
		a_n_plus_1 = this->series->operator()(i + 1);
		tmp = -a_n_plus_1 * a_n_plus_1;

		//formula[6]
		T_n[i] = fma(a_n * a_n_plus_1, (a_n + a_n_plus_1) / (fma(a_n, a_n, tmp) - fma(a_n_plus_1, a_n_plus_1, tmp)), this->series->S_n(i));
	}
	std::vector<T> T_n_plus_1(n + order, static_cast<T>(0));
	T a, b, c;
	for (K j = 2; j <= order; ++j) {
		for (K i = n_minus_order + j; i <= n_plus_order - j; ++i) { // int -> K
			a = T_n[i];
			b = T_n[i - 1];
			c = T_n[i + 1];
			T_n_plus_1[i] = static_cast<T>(std::fma(std::fma(a, c + b - a, -b * c), 1 / (std::fma(2, a, -b - c)), a));
		}
		T_n = T_n_plus_1;
	}

	if (!isfinite(T_n[n]))
		throw std::overflow_error("division by zero");

	return T_n[n];
}

/**
* @brief Shanks transformation for alternating series class.
* @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
*/
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class shanks_transform_alternating : public series_acceleration<T, K, series_templ>
{
public:

	/**
   * @brief Parameterized constructor to initialize the Shanks transformation for alternating series.
   * @authors Bolshakov M.P.
   * @param series The series class object
   */
	explicit shanks_transform_alternating(const series_templ& series);

	/**
   * @brief Shanks transformation for alternating series function.
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @return The partial sum after the transformation.
   */
	T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
shanks_transform_alternating<T, K, series_templ>::shanks_transform_alternating(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T shanks_transform_alternating<T, K, series_templ>::operator()(const K n, const K order) const {

	using std::isfinite;
	using std::fma;

	if (order == static_cast<K>(0)) [[unlikely]] /*it is convenient to assume that transformation of order 0 is no transformation at all*/
		return this->series->S_n(n);

	if (n < order || n == static_cast<K>(0)) [[unlikely]]
		return static_cast<T>(0); // TODO: диагностика

	if (order == static_cast<K>(1)) [[unlikely]]
	{
		T a_n, a_n_plus_1, result;

		a_n = this->series->operator()(n);
		a_n_plus_1 = this->series->operator()(n + 1);
		result = fma(a_n * a_n_plus_1, static_cast<T>(1) / (a_n - a_n_plus_1), this->series->S_n(n));

		if (!isfinite(result))
			throw std::overflow_error("division by zero");

		return result;
	}
	//n > order >= 1

	const K n_minus_order = n - order;
	const K n_minus_order1 = n_minus_order + 1;
	const K n_plus_order = n + order;

	std::vector<T> T_n(n_plus_order, static_cast<T>(0));

	T a_n, a_n_plus_1;

	a_n = this->series->operator()(n_minus_order);
	a_n_plus_1 = this->series->operator()(n_minus_order1);

	for (K i = n_minus_order1; i <= n_plus_order - 1; ++i) // if we got to this branch then we know that n >= order - see previous branches int->K
	{
		a_n = this->series->operator()(i);
		a_n_plus_1 = this->series->operator()(i + 1);

		// formula [6]
		T_n[i] = fma(a_n * a_n_plus_1, static_cast<T>(1) / (a_n - a_n_plus_1), this->series->S_n(n));
	}

	std::vector<T> T_n_plus_1(n_plus_order, static_cast<T>(0));
	T a, b, c;

	for (K j = 2; j <= order; ++j) {
		for (K i = n_minus_order + j; i <= n_plus_order - j; ++i) { // int -> K

			a = T_n[i];
			b = T_n[i - 1];
			c = T_n[i + 1];

			T_n_plus_1[i] = fma(fma(a, c + b - a, -b * c), static_cast<T>(1) / (static_cast<T>(2) * a - b - c), a);
		}
		T_n = T_n_plus_1;
	}

	if (!isfinite(T_n[n]))
		throw std::overflow_error("division by zero");

    return T_n[n];
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                        SHANKS TRANSFORMATION                                                           //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                        THETA MODIFIED ALGORITHM                                                        //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

 /**
  * @brief Theta Algorithm class template.
  * @tparam T Element type of the series.
  * @tparam K Integer index type.
  * @tparam series_templ Series type to accelerate.
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class theta_modified_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
	 * @brief Parameterized constructor to initialize the Theta Algorithm.
	 * @authors Novak.M., Maximov.A.K.
	 * @param series The series class object to be accelerated
	 */
	explicit theta_modified_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	 * @brief modified Theta Algorithm
	 * Computes the partial sum after the transformation using the theta_modified_algorithm
	 * @param n The number of terms in the partial sum.
	 * @param order The order of transformation.
	 * @return The partial sum after the transformation.
	 */

	T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T theta_modified_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	using std::isfinite;

	if (n < static_cast<K>(2)) return static_cast<T>(0);

	K m = n - static_cast<K>(1);

	K i1, i2;
	T delta, delta_next;

	std::vector<T> current(n);
	std::vector<T> next(n);

	for (K i = 0; i < n; ++i)
		current[i] = this->series->operator()(i);

	for (K k = 0; k < 1; ++k) { 
		for (K i = 0; i < m; ++i) {
			i1 = i + static_cast<K>(1);
			i2 = i + static_cast<K>(2);

			delta = current[i1] - current[i];
			delta_next = (i2 < n) ? current[i2] - current[i1] : static_cast<T>(0);

			next[i] = current[i1];
			next[i]+= (delta * delta_next) / (delta - delta_next);
		}
		current = next;
	}
	
	if (!isfinite(current[m - 1]))
		throw std::overflow_error("division by zero");
	return current[m - 1];
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                        THETA MODIFIED ALGORITHM                                                        //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                         RICHARDSON ALGORITHM                                                           //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

 /**
 * @brief Richardson transformation
 * @authors Trudolyubov N.A., Pavlova A.R.
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 * @tparam series_templ is the type of series whose convergence we accelerate
 */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class richardson_algorithm : public series_acceleration<T, K, series_templ>
{
public:
     /**
      * @brief Parameterized constructor to initialize the Richardson transformation for series.
      * @param series The series class object
      */
    explicit richardson_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}
     /**
      * @brief Richardson transformation for series function.
      * @param n The number of terms in the partial sum.
      * @param order The order of transformation.
      * @return The partial sum after the transformation.
      */
    T operator() (const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T richardson_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;
    using std::fma;

    // in the method we don't use order, it's only a stub 
    if (n == static_cast<K>(0)) return static_cast<T>(0); // TODO: диагностика

    std::vector<std::vector<T>> e(2, std::vector<T>(n + static_cast<K>(1), static_cast<T>(0))); //2 vectors n + 1 length containing Richardson table next and previous 
    
    for (K i = 0; i <= n; ++i)
        e[0][i] = this->series->S_n(i);

    // The Richardson method main function 
    T a, b;
    a = static_cast<T>(1);
    for (K l = 1; l <= n; ++l) {
        a *= static_cast<T>(4);
        b = a - static_cast<T>(1);

        for (K m = l; m <= n; ++m){
            e[1][m] = fma(a, e[0][m], -e[0][m - 1]);
            e[1][m]/= b;
        }

        std::swap(e[0], e[1]);
    }

    if (!isfinite(e[n & 1][n])) // get n & 1, cause if n is even, result is e[0][n], if n is odd, result is e[1][n]
        throw std::overflow_error("division by zero");

    return e[n & 1][n];
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                         RICHARDSON ALGORITHM                                                           //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                          LUBKIN W ALGORITHM                                                            //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

/**
 * @brief W_transformation class template.
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 * @tparam series_templ is the type of series whose convergence we accelerate
 */

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class W_lubkin_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	/**
	 * @brief Default function to calculate W-tranformation. 
	 * For more information see p. 290 15.4.1 [http://servidor.demec.ufpr.br/CFD/bibliografia/MER/Sidi_2003.pdf]
	 * @authors Yurov P.I. Bezzaborov A.A.
	 * @param n The number of terms in the partial sum.
	 * @param order the order of transformation
	 * @return The partial sum after the transformation.
	 */

	T calculate(K n, const K order, T S_n, const K j) const;

public:

	/**
	 * @brief Parameterized constructor to initialize the Lubkin W-transformation.
	 * @param series The series class object to be accelerated
	 * @param func Remainder function
	*/

	explicit W_lubkin_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}


	/**
     * @brief W-transformation.
     * Computes the partial sum after the W-transformation
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
    */

	T operator()(const K n, const K order) const override;
};

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T W_lubkin_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	if (order < static_cast<K>(0)) 
		throw std::domain_error("negative order input");

	return calculate(n, order, this->series->S_n(n), 0);
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T W_lubkin_algorithm<T, K, series_templ>::calculate(K n, const K order, T S_n, const K j) const {

	using std::isfinite;
	using std::fma;

	/*
	* j - to fix n
	* S_n - partial sum of series.
	*/
	for (K i = 0; i < j; ++i) 
		S_n += this->series->operator()(n  + i + static_cast<K>(1));
	
	n += j;

	if (order == static_cast<K>(0)) return S_n;

	//calculate all basic parts of transfor
	K order1 = order - static_cast<K>(1);
	T W0 = calculate(n, order1, S_n, 0);
	T W1 = calculate(n, order1, S_n, 1);
	T W2 = calculate(n, order1, S_n, 2);
	T W3 = calculate(n, order1, S_n, 3);
	
	//optimization calculations
	T Wo0 = W1 - W0;
	T Wo1 = W2 - W1;
	T Wo2 = W3 - W2;
	T Woo1 = Wo0 * (Wo2 - Wo1);
	T Woo2 = Wo2 * (Wo1 - Wo0);

	//T result = W1 - ((W2 - W1) * (W1 - W0) * (W3 - 2 * W2 + W1)) / ((W3 - W2) * (W2 - 2 * W1 + W0) - (W1 - W0) * (W3 - 2 * W2 + W1)); //straigh
	const T result = fma(-Wo1, Woo1 / (Woo2 - Woo1), W1); // optimized
	if (!isfinite(result))
		throw std::overflow_error("division by zero");
	return result;
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                          LUBKIN W ALGORITHM                                                            //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                                 FSA                                                                    //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class ford_sidi_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	std::unique_ptr<const shanks_transform<T, K, series_templ>> shanks_trans;

	/**
	 * @brief Recursive function to compute psi.
	 * Computes the value of psi according to the given parameters.
	 * @param n The number of terms in the partial sum.
	 * @param k The order of transformation.
	 * @return The value of psi.
	 */

	T Psi(
		const K n, 
		const K k, 
		const series_base<T,K>* u, 
		const shanks_transform<T, K, series_templ>* g
	) const;

	T Psi(
		const K n, 
		const K k, 
		const K k_1, 
		const shanks_transform<T, K, series_templ>* g
	) const;

public:

	/*
	* @brief Parameterized constructor to initialize the Ford-Sidi Algorithm.
    * @authors Matkov N.K. Peters E.A.
    * @param series The series class object to be accelerated
	*/
	explicit ford_sidi_algorithm(const series_templ& series);
	
	/*
	* @brief Fast implementation of Ford-Sidi.
    * @param n The number of terms in the partial sum.
    * @param order The order of transformation.
    * @return The partial sum after the transformation.
	*/
	T operator()(const K n, const K k) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T ford_sidi_algorithm<T, K, series_templ>::Psi(
	const K n, 
	const K k, 
	const series_base<T,K>* u, 
	const shanks_transform<T, K, series_templ>* g
) const {
	if (k == static_cast<K>(0))
		return (u->operator()(n)) / (g->operator()(n, 1));
	
	K n1  = n + static_cast<K>(1);
	K km1 = k - static_cast<K>(1);
	K kp1 = k + static_cast<K>(1);
	return (Psi(n1, km1, u, g) - Psi(n, km1, u, g)) / (Psi(n1, km1, kp1, g) - Psi(n, km1, kp1, g));
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T ford_sidi_algorithm<T, K, series_templ>::Psi(
	const K n, 
	const K k, 
	const K k_1, 
	const shanks_transform<T, K, series_templ>* g
) const {
	if (k == static_cast<K>(0))
		return (g->operator()(n, k_1)) / (g->operator()(n, 1));
	
	K n1  = n + static_cast<K>(1);
	K km1 = k - static_cast<K>(1);
	K kp1 = k + static_cast<K>(1);

	return (Psi(n1, km1, k_1, g) - Psi(n, km1, k_1, g)) / (Psi(n1, km1, kp1, g) - Psi(n, km1, kp1, g));
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T ford_sidi_algorithm<T, K, series_templ>::operator()(const K n, const K k) const 
{
	std::unique_ptr<one_series<T, K>> ones_seq(new one_series<T,K>(1));
	
	// Насколько мы коллективным разумом поняли, g это как бы ряд используемого преобразования(shanks_transform, richardson_extrapolation, G_transformation и тд)
	// пока я не реализовал выбор и оставил только shanks_transform, хотя в дальнейшем выбор конечно же надо прикрутить
	
	K k1 = k - static_cast<K>(1);
	K i1;
	T T_n_k = Psi(2, k1, (this->series), shanks_trans.get()) - Psi(1, k1, (this->series), shanks_trans.get());
	T_n_k /= (Psi(2, k1, ones_seq.get(), shanks_trans.get()) - Psi(1, k1, ones_seq.get(), shanks_trans.get()));
	
	for (K i = static_cast<K>(2); i <= n; ++i)
	{
		i1 = i + static_cast<K>(1);
		T a = Psi(i1, k1, (this->series), shanks_trans.get()) - Psi(i, k1, (this->series), shanks_trans.get());
		a /= (Psi(i1, k1, ones_seq.get(), shanks_trans.get()) - Psi(i, k1, ones_seq.get(), shanks_trans.get()));
		T_n_k += a;
	}

	return T_n_k;
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
ford_sidi_algorithm<T, K, series_templ>::ford_sidi_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series)
{
	shanks_trans.reset(new shanks_transform<T, K, series_templ>(this->series));
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                                 FSA                                                                    //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                                FSA2                                                                    //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class ford_sidi_algorithm_two : public series_acceleration<T, K, series_templ>
{
public:

	/*
	* @brief Parameterized constructor to initialize the Ford-Sidi V-2 Algorithm.
	* @authors Maximov A.K.
	* @param series The series class object to be accelerated
	*/
	explicit ford_sidi_algorithm_two(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/*
	* @brief Fast implementation of Ford-Sidi.
	* @param n The number of terms in the partial sum.
	* @param order The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K n, const K k) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T ford_sidi_algorithm_two<T, K, series_templ>::operator()(const K n, const K k) const {

	using std::fma;
	using std::isfinite;

	if (n == static_cast<K>(0))
		throw std::domain_error("n = 0 in the input");

	T delta_squared_S_n;
	K m = n;

	do{
		delta_squared_S_n = this->series->S_n(m + static_cast<K>(2));
		delta_squared_S_n-= static_cast<T>(2) * this->series->S_n(m + static_cast<K>(1));
		delta_squared_S_n+= this->series->S_n(m);
	} while (delta_squared_S_n == 0 && --m > static_cast<K>(0));

	if (m == static_cast<K>(0))
		throw std::overflow_error("division by zero");

	T delta_S_n = this->series->S_n(m + static_cast<K>(1));
	delta_S_n -= this->series->S_n(m);

	const T T_n = fma(-delta_S_n, delta_S_n / delta_squared_S_n, this->series->S_n(m));
	
	if (!isfinite(T_n))
		throw std::overflow_error("division by zero");
	return T_n;
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                                FSA2                                                                    //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                                FSA3                                                                    //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class ford_sidi_algorithm_three : public series_acceleration<T, K, series_templ>{
public:

    /*    
     * @brief Parameterized constructor to initialize the Ford-Sidi V-3 Algorithm.
     * @authors Sharonov K.S.    
     * @param series The series class object to be accelerated
     */    
    explicit ford_sidi_algorithm_three(const series_templ& series);

    /*
     * @brief Fast implementation of Ford-Sidi.    
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.    
     * * @return The partial sum after the transformation.
     */    
    T operator()(const K n, const K k) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
ford_sidi_algorithm_three<T, K, series_templ>::ford_sidi_algorithm_three(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T ford_sidi_algorithm_three<T, K, series_templ>::operator()(const K n, const K order) const {  

    using std::isfinite;

    if (n == static_cast<K>(0))
        throw std::domain_error("n = 0 in the input");
    
    //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения
    const K n1 = n - static_cast<K>(1);
    const K m  = n + static_cast<K>(1);

    std::vector<T> G(m + static_cast<K>(1));
    std::vector<T> FSA(G.size());
    std::vector<T> FSI(G.size());
    std::vector<std::vector<T>> FSG(m + static_cast<K>(2), std::vector<T>(G.size()));

    G[0] = this->series->operator()(n1) * static_cast<T>(n);
    
    T Te = static_cast<T>(1) / static_cast<T>(n);
    
    for (K k = static_cast<K>(1); k <= m; ++k)
        G[k] = Te * G[k - 1];

    FSA[n1] = this->series->S_n(n1);
    FSI[n1] = static_cast<T>(1);

    if (G[0] != static_cast<T>(0)) {
        FSA[n1] /= G[0];
        FSI[n1] /= G[0];
        for (K i = static_cast<K>(1); i <= m; ++i)
            FSG[i][n1] = G[i] / G[0];
    } 
    else
        for (K i = static_cast<K>(1); i <= m; ++i)
            FSG[i][n1] = G[i];

    //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения
    K MM, MM1, k2;
    T D;

    // Основной цикл алгоритма Ford-Sidi
    for (K k = static_cast<K>(0); k <= n1; ++k) {
        MM = n1 - k;
        MM1 = MM + static_cast<K>(1);
        k2 = k + static_cast<K>(2);
        D = FSG[k2][MM1] - FSG[k2][MM];

        for (K i = k + static_cast<K>(3); i <= m; ++i)
            FSG[i][MM] = (FSG[i][MM1] - FSG[i][MM]) / D;

        FSA[MM] = (FSA[MM1] - FSA[MM]) / D;
        FSI[MM] = (FSI[MM1] - FSI[MM]) / D;
    }

    FSA[0] /= FSI[0]; //result

    if (!isfinite(FSA[0]))
        throw std::overflow_error("division by zero");

    return FSA[0];
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                      EPSILON MODIFIED ALGORITHM                                                        //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//

 /**
  * @brief Epsilon Algorithm class template.
  * @tparam T Element type of the series.
  * @tparam K Integer index type.
  * @tparam series_templ Series type to accelerate.
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class epsilon_modified_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
	 * @brief Parameterized constructor to initialize the Epsilon Algorithm.
	 * @authors Novak.M., Maximov.A.K.
	 * @param series The series class object to be accelerated
	 */
	explicit epsilon_modified_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	 * @brief modified Epsilon Algorithm
	 * Computes the partial sum after the transformation using the epsilon_modified_algorithm
	 * @param n The number of terms in the partial sum.
	 * @param order The order of transformation.
	 * @return The partial sum after the transformation.
	 */

	T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T epsilon_modified_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	using std::isfinite;

	std::vector<std::vector<T>> eps(n, std::vector<T>(n, static_cast<T>(0)));

	for (K i = static_cast<K>(0); i < n; ++i)
		eps[i][0] = this->series->operator()(i);

	K m1, k1;

	for (K k = static_cast<K>(1); k < n; ++k)
	
		for (K m = k; m < n; ++m) {
			m1 = m - static_cast<K>(1);
			k1 = k - static_cast<K>(1);
			eps[m][k] = eps[m1][k1];
			eps[m][k] += static_cast<T>(1) / (eps[m][k1] - eps[m1][k1]) / (
				(k & static_cast<K>(1)) ? 
				static_cast<T>(order) + static_cast<T>(k + static_cast<K>(1)) / static_cast<T>(2) 
				: 
				static_cast<T>(2) / static_cast<T>(k + static_cast<K>(2))
			);

		}

	if (!isfinite(eps[n - static_cast<K>(1)][n - static_cast<K>(1)]))
		throw std::overflow_error("division by zero");

	return eps[n - static_cast<K>(1)][n - static_cast<K>(1)];
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                           EPSILON ALGORITHM                                                            //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

 /**
  * @brief Epsilon Algorithm class template.
  * @tparam T Element type of the series.
  * @tparam K Integer index type.
  * @tparam series_templ Series type to accelerate.
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class epsilon_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
     * @brief Parameterized constructor to initialize the Epsilon Algorithm.
     * @authors Pashkov B.B.
     * @param series The series class object to be accelerated
     */
	explicit epsilon_algorithm(const series_templ& series);

	/**
     * @brief Shanks multistep epsilon algorithm.
     * Computes the partial sum after the transformation using the Epsilon Algorithm.
     * For more information, see p. 5.3.2 in [https://e-maxx.ru/bookz/files/numerical_recipes.pdf]
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */
    T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
epsilon_algorithm<T, K, series_templ>::epsilon_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T epsilon_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	using std::isfinite;

	if (order == static_cast<K>(0))
		return this->series->S_n(n);

	const K m = static_cast<K>(2) * order;
	K max_ind = m + n; // int -> K mark

	const K n1 = n - static_cast<K>(1);

	std::vector<T> e0(max_ind + static_cast<K>(1), static_cast<T>(0));
	std::vector<T> e1(max_ind,     static_cast<T>(0));

	auto e0_add = &e0; // Pointer for vector swapping
	auto e1_add = &e1; // Pointer for vector swapping

	K j = max_ind;
	do {
		e0[j] = this->series->S_n(j);
	} while (--j > static_cast<K>(0));

	for (K i = static_cast<K>(0); i < m; ++i) {
		for (K j = n1; j < max_ind; ++j)
			(*e1_add)[j] += static_cast<T>(1) / ((*e0_add)[j + static_cast<K>(1)] - (*e0_add)[j]);

		--max_ind;
		std::swap(e0_add, e1_add);
		(*e1_add).erase((*e1_add).begin());
	}

	if (!isfinite((*e0_add)[n1]))
		throw std::overflow_error("division by zero");

	return (*e0_add)[n1];
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                           EPSILON ALGORITHM                                                            //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                         EPSILON ALGORITHM 2                                                            //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

/**
  * @brief Epsilon Algorithm MK-2 class template. "Scalar Epsilon Algorithm"
  //SOME RESULTS CONCERNING THE FUNDAMENTAL NATURE OF WYNN'S VECTOR EPSILON ALGORITHM - same algo + vector form
  //On a Device for Computing the e (S ) Transformation - nothing new, just matrix
  //euler algoritm A_Note_on_the_Generalised_Euler_Transformation-Wynn-1971 - has Euler, but for um = z^m * v_m
  * @authors Kreinin R.G.
  * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
*/
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class epsilon_algorithm_two : public series_acceleration<T, K, series_templ>
{
public:
    /**
     * @brief Parameterized constructor to initialize the Epsilon Algorithm MK-2.
     * @param series The series class object to be accelerated
    */
    explicit epsilon_algorithm_two(const series_templ& series);

    /**
     * @brief Fast impimentation of Levin algorithm.
     * Computes the partial sum after the transformation using the Epsilon Algorithm.
     * For more information, see page 20-21 in [https://hal.science/hal-04207550/document]
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
    */
    T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
epsilon_algorithm_two<T, K, series_templ>::epsilon_algorithm_two(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T epsilon_algorithm_two<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;

    if (n == static_cast<K>(0)) 
        throw std::domain_error("n = 0 in the input");

    if (order == static_cast<K>(0))
        return this->series->S_n(n);

    K k = static_cast<K>(2);
    k *= order;
    k += n;
    k -= (n & 1);

    std::vector<std::vector<T>> e(4, std::vector<T>(k + 3, static_cast<T>(0))); //4 vectors k+3 length containing four Epsilon Table rows 

    K j = k;
    do { //Counting first row of Epsilon Table
        e[3][j] = this->series->S_n(j);
    } while (--j > static_cast<K>(0));

    T a = static_cast<T>(0);

    K i1, i2;

    while (k > static_cast<K>(-1)) {
        for (K i = static_cast<K>(0); i < k; ++i) {
            i1 = i + static_cast<K>(1);
            i2 = i + static_cast<K>(2);
            e[0][i] = e[2][i1] + static_cast<T>(1) / (e[3][i1] - e[3][i]); //Standart Epsilon Wynn algorithm

            if (!isfinite(e[0][i]) && i2 <= k) { //This algorithm is used if new elliment is corrupted.
                a = e[2][i2] * static_cast<T>(1) / (static_cast<T>(1) - e[2][i2] / e[2][i1]);

                a += e[2][i] * static_cast<T>(1) / (static_cast<T>(1) - e[2][i] / e[2][i1]);

                a -= e[0][i2] * static_cast<T>(1)/ (static_cast<T>(1) - e[0][i2] / e[2][i1]);

                e[0][i] = a / (static_cast<T>(1) + a / e[2][i1]);
            }

            if (!isfinite(e[0][i])) //If new element is still corrupted we just copy prev. element, so we will get result
                e[0][i] = e[2][i];         
        }

        std::swap(e[0], e[1]); //Swapping rows of Epsilon Table. First ine will be overwriteen next turn
        std::swap(e[1], e[2]);
        std::swap(e[2], e[3]);

        --k;
    }
    
    if (!isfinite(e[0][0]))
        throw std::overflow_error("division by zero");

    return e[0][0];  //Only odd rows have mathmatical scense. Always returning e[0][0]
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                         EPSILON ALGORITHM 2                                                            //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                         EPSILON ALGORITHM 3                                                            //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

 /**
  * @brief MK-3 Epsilon Algorithm class template.
  * @tparam T Element type of series
  * @tparam K Index type
  * @tparam series_templ Series type to accelerate
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class epsilon_algorithm_three : public series_acceleration<T, K, series_templ>
{
private:

    const T epsilon_threshold; // Threshold for epsilon, defaults to 1e-3

public:
	/**
	* @brief Parameterized constructor to initialize the Epsilon Algorithm MK-2.
	* @param series The series class object to be accelerated
	*/
    explicit epsilon_algorithm_three(const series_templ& series, const T epsilon_threshold_ = static_cast<T>(1e-3));

	/**
	* @brief Fast impimentation of Epsilon algorithm.
	* Computes the partial sum after the transformation using the Epsilon Algorithm.
	* For more information, see 612.zip [https://calgo.acm.org/]
	* @param n The number of terms in the partial sum.
	* @param order The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
epsilon_algorithm_three<T, K, series_templ>::epsilon_algorithm_three(
    const series_templ& series, 
    const T epsilon_threshold_
    ) : 
    series_acceleration<T, K, series_templ>(series), 
    epsilon_threshold(epsilon_threshold_) 
{}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T epsilon_algorithm_three<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;
    using std::max;
    using std::abs;

    if (n == static_cast<K>(0)) 
        throw std::domain_error("n = 0 in the input");

    if (order == static_cast<K>(0)) return this->series->S_n(n);

    K N = n; // int -> K

    const T EMACH = std::numeric_limits<T>::epsilon(); // The smallest relative spacing for the T
    const T EPRN = static_cast<T>(50) * EMACH;
    const T OFRN = std::numeric_limits<T>::max(); //The largest finite magnitude that can be represented by a T 

    T result = static_cast<T>(0); //New result
    T abs_error = static_cast<T>(0); //Absolute error
    T resla = static_cast<T>(0); //Last result
    K newelm, num, NUM, K1, ib, ie, in;
    T RES, E0, E1, E2, E3, E1ABS, DELTA1, DELTA2, DELTA3, ERR1, ERR2, ERR3, TOL1, TOL2, TOL3, SS, EPSINF; // int -> K

    std::vector<T> e(N + static_cast<K>(3), static_cast<T>(0)); //First N eliments of epsilon table + 2 elements for math

    for (K i = static_cast<K>(0); i <= N; ++i) //Filling up Epsilon Table
        e[i] = this->series->S_n(i);

    for (K i = static_cast<K>(0); i <= order; ++i) { //Working with Epsilon Table order times

        num = NUM = K1 = N = n;
        K NEWELM = newelm = (N - static_cast<K>(1)) / static_cast<K>(2);
        e[N + static_cast<K>(2)] = e[N];
        e[N] = abs_error = OFRN;

        for (K I = static_cast<K>(1); I <= NEWELM; ++I) { //Counting all diagonal elements of epsilon table

            RES = e[K1 + static_cast<K>(2)];
            E0 = e[K1 - static_cast<K>(2)];
            E1 = e[K1 - static_cast<K>(1)];

            E2 = RES;

            E1ABS = static_cast<T>(abs(E1));
            
            DELTA2 = E2 - E1;

            ERR2 = static_cast<T>(abs(DELTA2));

            TOL2 = max(
                static_cast<T>(abs(E2)), 
                E1ABS
            );
            TOL2*=EMACH;

            DELTA3 = E1 - E0;
            ERR3 = static_cast<T>(abs(DELTA3));
            TOL3 = max(
                E1ABS, 
                static_cast<T>(abs(E0))
            );
            TOL3*= EMACH;

            if (ERR2 > TOL2 || ERR3 > TOL3) {

                E3 = e[K1];
                e[K1] = E1;

                DELTA1 = E1;
                DELTA1-= E3;

                ERR1 = static_cast<T>(abs(DELTA1));

                TOL1 = max(
                    E1ABS, 
                    static_cast<T>(abs(E3))
                );
                TOL1*= EMACH;

                if (ERR1 <= TOL1 || ERR2 <= TOL2 || ERR3 <= TOL3) {
                    N = static_cast<K>(2) * I - static_cast<K>(1);
                    break;
                }

                SS = static_cast<T>(1) / DELTA1 + static_cast<T>(1) / DELTA2 - static_cast<T>(1) / DELTA3;
                EPSINF = static_cast<T>(abs(SS * E1));

                if (EPSINF > epsilon_threshold) {
                    RES = E1 + static_cast<T>(1) / SS;
                    e[K1] = RES;
                    K1 -= static_cast<K>(2);
                    T ERROR = ERR2 + static_cast<T>(abs(RES - E2)) + ERR3;
                    if (ERROR <= abs_error) {
                        abs_error = ERROR;
                        result = RES;
                    }
                }
                else {
                    N = static_cast<K>(2) * I - static_cast<K>(1);
                    break;
                }
            }

            else {
                result = RES;
                abs_error = ERR2 + ERR3;
                e[K1] = result;
                break;
            }
        }

        if (N == n) // making N the greatest odd number <= n
            N = (n & 1) ? n : n - static_cast<K>(1);

        ib = (num & 1) ? static_cast<K>(1) : static_cast<K>(2);  // Start index: 1 for odd, 2 for even
        ie = newelm + static_cast<K>(1);

        // Copy elements with step 2
        for (K pos = ib; pos < ib + static_cast<K>(2) * ie; pos += static_cast<K>(2))
            e[pos] = e[pos + static_cast<K>(2)];

        if (num != N) {
            in = num - N + static_cast<K>(1);
            for (K j = static_cast<K>(1); j <= N; ++j, ++in)
                e[j] = e[in];
        }

        abs_error = max(
            static_cast<T>(abs(result - resla)), 
            EPRN * static_cast<T>(abs(result))
        );

        resla = result;
    }

    if (!isfinite(result))
        throw std::overflow_error("division by zero");

    return result;
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                         EPSILON ALGORITHM 3                                                            //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                    EPSILON AITKEN THETA ALGORITHM                                                      //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

 /**
  * @brief Epsilon-Aitken-Theta Algorithm class template.
  * @tparam T Element type of the series.
  * @tparam K Integer index type.
  * @tparam series_templ Series type to accelerate.
*/
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class epsilon_aitken_theta_algorithm : public series_acceleration<T, K, series_templ>
{
public:
    /**
     * @brief Parameterized constructor to initialize the Theta Algorithm.
     * @authors Novak.M., Maximov.A.K.
     * @param series The series class object to be accelerated
    */
    epsilon_aitken_theta_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

    /**
     * @brief combizing of Epsilon Aitken Theta algorithms
     * Computes the partial sum after the transformation using the Epsilon, Aitken, Theta algo
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
    */

    T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T epsilon_aitken_theta_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;

    if (n < static_cast<K>(4))
        return static_cast<T>(0); //TODO: диагностический вывод

    std::vector<T> current(n);

    for (K i = static_cast<K>(0); i < n; ++i)
        current[i] = this->series->operator()(i);

    T delta, delta_next;
    T gamma, lambda;
    T ck = static_cast<T>(1);
    K i1, i2;

    for (K k = static_cast<K>(1); k <= n; ++k) { 

        std::vector<T> step1(n, static_cast<T>(0));
        std::vector<T> step3(n, static_cast<T>(0));

        switch (order){
            case static_cast<K>(1):
                gamma = static_cast<T>(1);
                gamma/= static_cast<T>(2);
                gamma/= static_cast<T>(k);

                lambda = static_cast<T>(1) - gamma;

                break;
            case static_cast<K>(2):

                gamma = static_cast<T>(2);
                gamma/= static_cast<T>(3);
                gamma/= static_cast<T>(k);

                lambda = static_cast<T>(1) - gamma;

                break;
            default:
                gamma = static_cast<T>(1);
                gamma/=(k + static_cast<T>(1));

                lambda =static_cast<T>(k);
                lambda*=gamma;
        }

        switch (order){
            case static_cast<K>(1):
                ck+= static_cast<T>(order /(static_cast<K>(2) * k - static_cast<K>(1)));
                break;

            case static_cast<K>(2):
                ck+= static_cast<T>(static_cast<K>(1)/(static_cast<K>(3) * k - static_cast<K>(2)));
                break;

            default:
                ck+=static_cast<T>(order / k);
        }

        step1[0] = current[1] - ck * (current[1] - current[0]) / (current[2] - current[1]);

        for (K i = static_cast<K>(0); i < static_cast<K>(step3.size() - 3); ++i) {

            i1 = i + static_cast<K>(1);
            i2 = i + static_cast<K>(2);

            step1[i1] = current[i2] - ck * (current[i2] - current[i1]) / (current[i2 + static_cast<K>(1)] - current[i2]);

            delta = step1[i1] - step1[i];

            delta_next = (i2 < static_cast<K>(step1.size()) ? 
                step1[i2] - step1[i1] 
                : 
                static_cast<T>(0)
            );

            step3[i] = gamma * step1[i] + lambda * (step1[i1] + (delta * delta_next) / (delta - delta_next));
        }

        current = step3;
    }

    if (!isfinite(current[n - static_cast<K>(4)]))
        throw std::overflow_error("division by zero");

    return current[n - static_cast<K>(4)];
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                    EPSILON AITKEN THETA ALGORITHM                                                      //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                        CHANG WHYNN ALGORITHM                                                           //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

/**
  * @brief Chang-Wynn algorithm class template. 
  //SOME RESULTS CONCERNING THE FUNDAMENTAL NATURE OF WYNN'S VECTOR EPSILON ALGORITHM - same algo + vector form
  //On a Device for Computing the e (S ) Transformation - nothing new, just matrix 
  //Construction of new generalizations of Wynn’s epsilon and rho algorithm by solving finite difference equations in the transformation order
  * @authors Kreinin R.G.
  * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
*/
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class chang_whynn_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
	 * @brief Parameterized constructor to initialize the chang_wynn_algorithm
	 * @param series The series class object to be accelerated
	*/
    explicit chang_whynn_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	 * @brief Fast impimentation of Levin algorithm.
	 * Computes the partial sum after the transformation using the chang_wynn_algorithm
	 * @param n The number of terms in the partial sum.
	 * @param order The order of transformation.
	 * @return The partial sum after the transformation.
	*/
	T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T chang_whynn_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;
    using std::fma;

    if (n == static_cast<K>(0))
        throw std::domain_error("n = 0 in the input");

    T up, down, coef, coef2;

    //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения
    K i1, i2, i3, k1;
    K max = n - (n & 1); // int -> K mark 

    std::vector<std::vector<T>> e(2, std::vector<T>(n, static_cast<T>(0))); //2 vectors n length containing Epsilon table next and previous 
    std::vector<T> f(n, static_cast<T>(0)); //vector for containing F results from 0 to n-1

    for (K i = static_cast<K>(0); i < max; ++i) //Counting first row of Epsilon Table
        e[0][i] = static_cast<T>(1) / (this->series->operator()(i + static_cast<K>(1)));

    for (K i = static_cast<K>(0); i < max; ++i) { //Counting F function

        i1 = i + static_cast<K>(1);
        i2 = i + static_cast<K>(2);
        i3 = i + static_cast<K>(3);

        coef = fma(
            static_cast<T>(-2), 
            this->series->S_n(i2), 
            this->series->S_n(i3) + this->series->S_n(i1)
        );
        coef2 = fma(
            static_cast<T>(-2), 
            this->series->S_n(i1), 
            this->series->S_n(i2) + this->series->S_n(i)
        );

        up = this->series->operator()(i1);
        up*= this->series->operator()(i2);
        up*= coef;

        down = this->series->operator()(i3) * coef2;
        down -= this->series->operator()(i1) * coef;
        down = static_cast<T>(1) / down;

        e[1][i] = static_cast<T>(fma(-up, down, this->series->S_n(i1)));
        f[i] = coef * coef2 * down; //Can make coeff2 ^2 for better effect
    }
    for (K k = static_cast<K>(2); k <= max; ++k) { //Counting from 2 to n rows of Epsilon Table
        k1 = static_cast<K>(1) - k;

        for (K i = static_cast<K>(0); i < max - k; ++i) {
            i1 = i + static_cast<K>(1);

            up = fma(static_cast<T>(k), f[i], static_cast<T>(k1));
            down = static_cast<T>(1) / (e[1][i1] - e[1][i]);

            e[0][i] = fma(up, down, e[0][i1]);
            if (!isfinite(e[0][i])) { //Check for invalid values to avoid them
                max = k + i1;
                break;
            }

        }
        
        std::swap(e[0], e[1]); //Swapping 1 and 2 rows of Epsilon Table. First ine will be overwriteen next turn
    }

    if (!isfinite(e[max & 1][0])) //Only odd rows have mathmatical scence. Always returning e[0][0]
        throw std::overflow_error("division by zero");

    return e[max & 1][0];
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                        CHANG WHYNN ALGORITHM                                                           //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                       BREZINSKI THETA ALGORITHM                                                        //
//                                                                BEGIN                                                                   //
//----------------------------------------------------------------------------------------------------------------------------------------//

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class theta_brezinski_algorithm : public series_acceleration<T, K, series_templ>
{
protected:
    /**
     * @brief Recursive function to compute theta.
     * Computes the value of theta according to the given parameters.
     * @param n The number of terms in the partial sum.
     * @param k The order of transformation.
     * @return The value of theta.
     */
    T theta(K n, const K order, T S_n, const K j) const;

public:
    /*
     * @brief Parameterized constructor to initialize the Theta Brezinski Algorithm.
     * @authors Yurov P.I. Bezzaborov A.A.
     * @param series The series class object to be accelerated
     */
    explicit theta_brezinski_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

    /**
     * @brief Fast implementation of Theta Brezinski algorithm.
     * Computes the partial sum after the transformation using the Theta Brezinski Algorithm.
     * For more information, see p. 277 10.2-4 in [https://arxiv.org/pdf/math/0306302.pdf]
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */
    T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T theta_brezinski_algorithm<T, K, series_templ>::theta(K n, const K order, T S_n, const K j) const {

    using std::isfinite;

    if (order == static_cast<K>(1)) {

        T res = static_cast<T>(1) / this->series->operator()(n + j + static_cast<K>(1));
        if (!isfinite(res))
            throw std::overflow_error("division by zero");
        return res;
    }

    for (K tmp = n + 1; tmp <= n + j; ++tmp)
        S_n += this->series->operator()(tmp);

    n += j;

    if (order == static_cast<K>(0))
        return S_n;

    //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения
    const K order1 = order - static_cast<K>(1);
    const K order2 = order - static_cast<K>(2);
    const T theta_order1_0 = theta(n, order1, S_n, 0);
    const T theta_order1_1 = theta(n, order1, S_n, 1);
    const T theta_order1_2 = theta(n, order1, S_n, 2);
    const T theta_order2_1 = theta(n, order2, S_n, 1);

    if (order & 1) { // order is odd
        const T delta = static_cast<T>(1) / (theta_order1_0 - theta_order1_1); // 1/Δυ_2k^(n)
        if (!isfinite(delta))
            throw std::overflow_error("division by zero");
        return theta_order2_1 + delta; // υ_(2k+1)^(n)=υ_(2k-1)^(n+1) + 1/(Δυ_2k^(n)
    }
    // order is even
    const T delta2 = static_cast<T>(1) / fma(static_cast<T>(-2), theta_order1_1, theta_order1_0 + theta_order1_2); // Δ^2 υ_(2k+1)^(n)

    if (!isfinite(delta2))
        throw std::overflow_error("division by zero");

    const T delta_n = theta_order2_1 - theta(n, order2, S_n, static_cast<K>(2)); // Δυ_2k^(n+1) 
    const T delta_n1 = theta_order1_1 - theta_order1_2; // Δυ_(2k+1)^(n+1)

    return fma(delta_n * delta_n1, delta2, theta_order2_1); // υ_(2k+2)^(n)=υ_2k^(n+1)+((Δυ_2k^(n+1))*(Δυ_(2k+1)^(n+1)))/(Δ^2 υ_(2k+1)^(n)
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T theta_brezinski_algorithm<T, K, series_templ>::operator()(const K n, const K order) const{
    if (order & 1) // is order odd?
        throw std::domain_error("order should be even number");

    if (n == static_cast<K>(0) || order == static_cast<K>(0))
        return this->series->S_n(n);

    return theta(n, order, this->series->S_n(n), 0);
}

//----------------------------------------------------------------------------------------------------------------------------------------//
//                                                       BREZINSKI THETA ALGORITHM                                                        //
//                                                                 END                                                                    //
//----------------------------------------------------------------------------------------------------------------------------------------//