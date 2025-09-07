/**
 * @file levin_algorithm.h
 * @brief This file contains the declaration of the Levin algorithm class.
 */

#pragma once

#include "series_acceleration.hpp" // Include the series header


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

		C_njk  = static_cast<T>(pow(n + j     + static_cast<K>(1), order - static_cast<K>(1)));
		C_njk /= static_cast<T>(pow(n + order + static_cast<K>(1), order - static_cast<K>(1)));

		S_nj = this->series->S_n(n + j);

		g_n = static_cast<T>(1);
		g_n/= (this->series->operator()(n + j));

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

    T w_n = static_cast<T>(static_cast<K>(1)-static_cast<K>(2)*(n_time % static_cast<K>(2))) * this->series->fact(n_time);
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

    const T result = (*this)(n, order, beta, false) / (*this)(n, order, beta, true);

    if (!isfinite(result)) throw std::overflow_error("division by zero");

    return result;
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T levin_L_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	using std::isfinite;

	if (n == static_cast<K>(0)) 
		throw std::domain_error("n = 0 in the input");

	if (order == static_cast<K>(0)) return this->series->S_n(n);

    const T result = (useRecFormulas ? calc_result_rec(n,order) : calc_result(n, order));
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}