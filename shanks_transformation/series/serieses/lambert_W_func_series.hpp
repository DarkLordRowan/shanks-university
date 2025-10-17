#pragma once

#include "../series_base.hpp"

#ifndef INC_FPRECISION
    #include "libs/arbitrary_arithmetics/fprecision.h"
#endif

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class lambert_W_func_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit lambert_W_func_series() : series_base<T, K>("lambert_W_func_series") {};

	virtual series_result<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const T& x){
		
		using std::isfinite;

        if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag()) || float_precision(abs(x)) >= float_precision(1) / float_precision(std::numbers::e);
        } else {
            return !isfinite(x) || abs(x) >= static_cast<T>(1) / static_cast<T>(std::numbers::e);
        }
	}

	inline constexpr T calculate_summ(const T& x){

		if constexpr (isComplexLike<T>::value){
            return static_cast<T>(0);
        } else if constexpr (std::is_same<T, float_precision>::value){
            return lambertW0(x);
        } else {
			return static_cast<T>(lambertW0(float_precision(x)));
		}

	}

};

template<AcceptedLike T, UnsignedIntLike K>
series_result<T> lambert_W_func_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or |x|>= 1/e");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_summ(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

	vecAn[0] = x;
	vecSn[0] = x;

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += static_cast<T>(-1) * vecAn[j-static_cast<K>(1)] * x / static_cast<T>(j);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return series_result<T>{.Sn = vecSn, .an = vecAn };

}