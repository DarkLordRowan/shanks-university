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

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;

        if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
            return !isfinite(x) || abs(x) >= float_precision(1) / float_precision(std::numbers::e);
        } else {
            return !isfinite(x) || abs(x) >= static_cast<T>(1) / static_cast<T>(std::numbers::e);
        }
	}

	inline constexpr T calculateSum(const T& x){

		if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
            return 0;
        } else if constexpr (std::is_same<T, float_precision>::value){
            return lambertW0(float_precision(x));
        } else {
			return static_cast<T>(lambertW0(float_precision(x, series_base<T,K>::precision)));
		}

	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> lambert_W_func_series<T, K>::generateSeries(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or |x|>= 1/e");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculateSum(x);

	if constexpr ( std::is_same<T, float_precision> :: value ){
		series_base<T, K>::precision = x.precision();
	} else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
		series_base<T, K>::precision = std::max(x.real().precision(), x.imag().precision());
	}

	std::vector<T> vecAn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecAn[0] = x;
	std::vector<T> vecSn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecSn[0] = x;

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += static_cast<T>(-1) * vecAn[j-static_cast<K>(1)] * x / static_cast<T>(j);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}