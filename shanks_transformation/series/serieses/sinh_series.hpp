#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class sinh_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit sinh_series() : series_base<T, K>("sinh_series") {};

	virtual SeriesResult<T> generateSeries(
        const T& x, 
		const K vecSize, 
		const T& addTParameter,
		const K addKParameter
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag());
		} else {
			return !isfinite(x);
		}
	}

	inline constexpr T calculateSum(const T& x){

		using std::sinh;

		return sinh(x);
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> sinh_series<T, K>::generateSeries(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculateSum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::initVecsWithPrec(vecSn,vecAn, vecSize, x);

	vecAn[0] = x;
	vecSn[0] = x;

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += vecAn[j-static_cast<K>(1)] * x * x / static_cast<T>(j * (static_cast<K>(4) * j + static_cast<K>(2)));
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}