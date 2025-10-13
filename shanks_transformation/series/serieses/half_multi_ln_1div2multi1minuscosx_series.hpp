#pragma once

#include "../series_base.hpp"
#include <cmath>

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class half_multi_ln_1div2multi1minuscosx_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit half_multi_ln_1div2multi1minuscosx_series() : series_base<T, K>("half_multi_ln_1div2multi1minuscosx_series") {};

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;

        if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag()) || abs(x) >= abs(static_cast<T>(std::numbers::pi));
        } else {
            return !isfinite(x) || abs(x) >= static_cast<T>(std::numbers::pi);
        }

	}

	inline constexpr T calculateSum(const T& x){

		using std::log;
		using std::cos;

		return static_cast<T>(-0.5) * log(static_cast<T>(2) - static_cast<T>(2) * cos(x));
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> half_multi_ln_1div2multi1minuscosx_series<T, K>::generateSeries(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or |x| >= pi");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculateSum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::initVecsWithPrec(vecSn,vecAn, vecSize, x);

    using std::cos;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += cos(static_cast<T>(j+1) * x) / static_cast<T>(j+1);
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}