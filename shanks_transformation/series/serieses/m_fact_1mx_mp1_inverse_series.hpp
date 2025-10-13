#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class m_fact_1mx_mp1_inverse_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit m_fact_1mx_mp1_inverse_series() : series_base<T, K>("m_fact_1mx_mp1_inverse_series") {};

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){

		using std::isfinite;
		
		if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag()) || abs(x) >= abs(static_cast<T>(1));
        } else {
		    return !isfinite(x) || abs(x) >= static_cast<T>(1);
        }


	}

	inline constexpr T calculateSum(const T& x, const K m){

        using std::pow;

        return static_cast<T>(fact<K>(m)) / pow(static_cast<T>(1) - x, static_cast<T>(m+1));
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> m_fact_1mx_mp1_inverse_series<T, K>::generateSeries(
    const T& x , 
	const K vecSize, 
	const T& addTParameter, //not needed
	const K addKParameter //m 
) {
	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or |x|>=1");
	}

    std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::initVecsWithPrec(vecSn,vecAn, vecSize, x);

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculateSum(x, addKParameter);
	vecAn[0] = static_cast<T>(fact<K>(addKParameter));
	vecSn[0] = static_cast<T>(fact<K>(addKParameter));

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += vecAn[j-1] * x * static_cast<T>(addKParameter + j) / static_cast<T>(j);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}