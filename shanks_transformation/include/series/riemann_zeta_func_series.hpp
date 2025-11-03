#pragma once

#include "series_base.hpp"
#include <type_traits>

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class riemann_zeta_func_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit riemann_zeta_func_series() : series_base<T, K>("riemann_zeta_func_series") {};

	virtual series_result<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const T& x){
		
		using std::isfinite;

        return !isfinite(x) || x < static_cast<T>(1);

	}

	inline T calculate_sum(const T& x){

		using std::to_string;

		std::string adapterX = to_string(x);

		return static_cast<T>(static_cast<double>(zeta(float_precision(adapterX))));

	}

};

template<AcceptedLike T, UnsignedIntLike K>
series_result<T> riemann_zeta_func_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or Re(x)<1");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_sum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

    using std::pow;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += pow(static_cast<T>(j+1), -x);
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return series_result<T>{.Sn = vecSn, .an = vecAn };

}

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <FloatLike T, UnsignedIntLike K>
class riemann_zeta_func_series<complex_precision<T>, K> final : public series_base<complex_precision<T>, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit riemann_zeta_func_series() : series_base<complex_precision<T>, K>("riemann_zeta_func_series") {};

	virtual series_result<complex_precision<T>> generate_series(
        const complex_precision<T>& x , 
		const K vecSize, 
		const complex_precision<T>& addTParameter = complex_precision<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const complex_precision<T>& x){
		
		using std::isfinite;

    	return !isfinite(x.real()) || !isfinite(x.imag()) || x.real() < static_cast<T>(1);
	}

	inline complex_precision<T> calculate_sum(const complex_precision<T>& x){

        return complex_precision<T>(0);

	}

};

template<FloatLike T, UnsignedIntLike K>
series_result<complex_precision<T>> riemann_zeta_func_series<complex_precision<T>, K>::generate_series(
    const complex_precision<T>& x , 
	const K vecSize, 
	const complex_precision<T>& addTParameter,
	const K addKParameter
) {

	using Complex = complex_precision<T>;

	if(check_domain(x)){
		series_base<Complex, K>::throw_domain_error("x is not finite or Re(x)<1");
	}

	series_base<Complex,K>::x_ = x;
	series_base<Complex,K>::sum = calculate_sum(x);

	std::vector<Complex> vecAn;
	std::vector<Complex> vecSn;

	series_base<Complex,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

    using std::pow;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += pow(Complex(j+1), -x);
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return series_result<Complex>{.Sn = vecSn, .an = vecAn };

}