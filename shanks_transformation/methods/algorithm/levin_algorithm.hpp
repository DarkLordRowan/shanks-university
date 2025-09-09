/**
 * @file levin_algorithm.hpp
 * @brief This file contains the declaration of the Levin algorithm class.
 */

#pragma once

#include "../series_acceleration.hpp"
#include "../remainders.hpp"

#include <cmath> //Include for pow, isfinite
#include <vector>
#include <memory>


/**
  * @brief Levin Algorithm class template.
  * @authors  Kreinin R.G., Trudolyubov N.A.
  * @tparam T Element type of the series
  * @tparam K Integer index type
  * @tparam series_templ Series type to accelerate
*/
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class levin_algorithm final : public series_acceleration<T, K, series_templ>
{
protected:

	T beta;
    std::unique_ptr<const transform_base<T, K>> remainder;
    bool useRecFormulas = false;
    remainder_type variant = remainder_type::u_variant;

	/**
	 * @brief 
	 * @param n     K The series class object to be accelerated
	 * @param order K The type of enumerating integer
	*/
	inline T calc_result(K n, K order) const;

	/**
	 * @brief 
	 * @param n     K The series class object to be accelerated
	 * @param order K The type of enumerating integer
	*/
	inline T calc_result_rec(K n, K order) const;

public:
	/**
	 * @brief Parameterized constructor to initialize the Levin Algorithm.
	 * @param series The series class object to be accelerated
	 * @param variant Type of remainder to use
	 * @param useRecFormulas use reccurence or straightforward formula 
	 * @param beta is nonzero positivbe real number, default value in literature is beta = 1, for more info p39 [https://arxiv.org/pdf/math/0306302]
	*/

	explicit levin_algorithm(
		const series_templ& series,
        remainder_type variant = remainder_type::u_variant,
        bool useRecFormulas = false,  
        T beta = static_cast<T>(1)
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
		if (beta > static_cast<T>(0))
			this->beta = beta;
		else this->beta = static_cast<T>(1);

	//check variant else default 'u'
    //TODO: тоже самое наверное
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
            remainder.reset(new u_transform<T, K>());
    }
	}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T levin_algorithm<T, K,series_templ>::calc_result(K n, K order) const{

	using std::pow;
	using std::isfinite;

	T numerator = static_cast<T>(0), denominator = static_cast<T>(0);
	T C_njk, S_nj, g_n, rest;

	for (K j = static_cast<K>(0); j <= order; ++j) { //Standart Levin algo procedure

		rest  = this->series->minus_one_raised_to_power_n(j);
		rest *= this->series->binomial_coefficient(static_cast<T>(order), j);

		C_njk  = static_cast<T>(pow(n + j     + static_cast<K>(1), order - static_cast<K>(1)));
		C_njk /= static_cast<T>(pow(n + order + static_cast<K>(1), order - static_cast<K>(1)));

		S_nj = this->series->S_n(n + j);

		g_n = static_cast<T>(1);
		g_n/= remainder->operator()(
            n + j, 
            j, 
            this->series,
            (variant == remainder_type::u_variant ? beta : static_cast<T>(1))
        );

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

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T levin_algorithm<T, K,series_templ>::calc_result_rec(K n, K order) const{

	using std::isfinite;

    //const T result = (*this)(n, order, beta, false) / (*this)(n, order, beta, true);

	std::vector<T>   Num(order + static_cast<K>(1), static_cast<T>(0));
	std::vector<T> Denom(order + static_cast<K>(1), static_cast<T>(0));

    //init the base values
	for (K i = static_cast<K>(0); i < order+static_cast<K>(1); ++i) {
		Denom[i] = remainder->operator()(
            n, 
            i, 
            this->series,
            (variant == remainder_type::u_variant ? beta : static_cast<T>(1))
        );

		Num[i] = this->series->S_n(n+i) * Denom[i];
	}

    //recurrence
	T scale, nj;
	const T order1 = static_cast<T>(order - static_cast<K>(1));

	for (K i = static_cast<K>(1); i <= order; ++i)
		for (K j = static_cast<K>(0); j <= order - i; ++j) {

			nj = static_cast<T>(n + j);

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

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T levin_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	using std::isfinite;

	if (n == static_cast<K>(0)) 
		throw std::domain_error("n = 0 in the input");

	if (order == static_cast<K>(0)) return this->series->S_n(n);

    const T result = (useRecFormulas ? calc_result_rec(n,order) : calc_result(n, order));
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}