#pragma once

#include "../series_base.hpp"
#include <cmath>

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class ln_sinx_minus_ln_x_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit ln_sinx_minus_ln_x_series() : series_base<T, K>("ln_sinx_minus_ln_x_series") {};

	virtual series_result<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const T& x){
		
		using std::isfinite;

        return !isfinite(x) || x > static_cast<T>(std::numbers::pi) ||  x < static_cast<T>(0);

	}

	inline constexpr T calculate_sum(const T& x){

		using std::log;
		using std::sin;

		return log(sin(x)) - log(x);
	}

};

template<AcceptedLike T, UnsignedIntLike K>
series_result<T> ln_sinx_minus_ln_x_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or Re(x)<0 or Re(x)>pi");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_sum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

	using std::log;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += log(static_cast<T>(1) - x * x  / (static_cast<T>((j+1)*(j+1)) * static_cast<T>(std::numbers::pi) * static_cast<T>(std::numbers::pi)));
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
class ln_sinx_minus_ln_x_series<complex_precision<T>, K> final : public series_base<complex_precision<T>, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit ln_sinx_minus_ln_x_series() : series_base<complex_precision<T>, K>("ln_sinx_minus_ln_x_series") {};

	virtual series_result<complex_precision<T>> generate_series(
        const complex_precision<T>& x , 
		const K vecSize, 
		const complex_precision<T>& addTParameter = static_cast<complex_precision<T>>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const complex_precision<T>& x){
		
		using std::isfinite;

    	return !isfinite(x.real()) || !isfinite(x.imag()) || x.real() > static_cast<T>(std::numbers::pi) || x.real() < static_cast<T>(0);
	}

	inline constexpr complex_precision<T> calculate_sum(const complex_precision<T>& x){

		using std::log;
		using std::sin;

		return log(sin(x)) - log(x);
	}

};

template<FloatLike T, UnsignedIntLike K>
series_result<complex_precision<T>> ln_sinx_minus_ln_x_series<complex_precision<T>, K>::generate_series(
    const complex_precision<T>& x , 
	const K vecSize, 
	const complex_precision<T>& addTParameter,
	const K addKParameter
) {

	using Complex = complex_precision<T>;

	if(check_domain(x)){
		series_base<Complex, K>::throw_domain_error("x is not finite or Re(x)<0 or Re(x)>pi");
	}

	series_base<Complex,K>::x_ = x;
	series_base<Complex,K>::sum = calculate_sum(x);

	std::vector<Complex> vecAn;
	std::vector<Complex> vecSn;

	series_base<Complex,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

	using std::log;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += log(Complex(1) - x * x  / (Complex((j+1)*(j+1)) * Complex(std::numbers::pi) * Complex(std::numbers::pi)));
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return series_result<Complex>{.Sn = vecSn, .an = vecAn };

}