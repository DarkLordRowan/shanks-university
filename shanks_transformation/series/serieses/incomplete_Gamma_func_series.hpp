#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class incomplete_Gamma_func_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit incomplete_Gamma_func_series() : series_base<T, K>("incomplete_Gamma_func_series") {};

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;

        if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag());
		} else {
			return !isfinite(x);
		}
	}

	inline constexpr T calculateSum(const T& x, const T& alpha){

		return static_cast<T>(0);
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> incomplete_Gamma_func_series<T, K>::generateSeries(
    const T& x, 
	const K vecSize, 
	const T& addTParameter, //s
	const K addKParameter
) {

    using std::to_string;

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculateSum(x, addTParameter); 
    series_base<T,K>::series_name += " " + to_string(addTParameter);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::initVecsWithPrec(vecSn,vecAn, vecSize, x);

    using std::pow;

	vecAn[0] = pow(x, addTParameter) / addTParameter;
	vecSn[0] = pow(x, addTParameter) / addTParameter;

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += static_cast<T>(-1) * vecAn[j-static_cast<K>(1)] * x * (addTParameter + static_cast<T>(j-1)) /((addTParameter + static_cast<T>(j)) * static_cast<T>(j));
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}