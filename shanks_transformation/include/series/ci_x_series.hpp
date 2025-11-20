#pragma once

#include "series_base.hpp"
#include <gsl/gsl_sf.h>
#include <gsl/gsl_sf_result.h>
#include <type_traits>

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class ci_x_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit ci_x_series() : series_base<T, K>("ci_x_series") {};

	virtual series_result<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domainn(const T& x){
		
		using std::isfinite;
		if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag());
		} else {
			return !isfinite(x);
		}
	}

	inline constexpr T calculate_sum(const T& x){

		if constexpr (std::is_floating_point<T>::value){
			return static_cast<T>(gsl_sf_Ci(static_cast<double>(x)));
		} else {
			return static_cast<T>(0);
		}

	}

};

template<AcceptedLike T, UnsignedIntLike K>
series_result<T> ci_x_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domainn(x)){
		series_base<T, K>::throw_domain_error("x is not finite");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_sum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

    using std::log;

    vecAn[0] = static_cast<T>(std::numbers::egamma);
    vecSn[0] = static_cast<T>(std::numbers::egamma);

    vecAn[1] = log(x);
    vecSn[1] += vecSn[0] + vecAn[1];

	vecAn[2] = x*x*static_cast<T>(-0.25);
	vecSn[2] += vecSn[1] + vecAn[2];

	for(K j = static_cast<K>(3); j < vecSize; ++j){
		vecAn[j] += static_cast<T>(-1)*vecAn[j-static_cast<K>(1)] * x * x / static_cast<T>(2*(j-1)*(j-1)*fma(2,j-2,1));
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return series_result<T>{.Sn = vecSn, .an = vecAn };

}