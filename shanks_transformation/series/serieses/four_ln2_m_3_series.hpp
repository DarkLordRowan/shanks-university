#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class four_ln2_m_3_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit four_ln2_m_3_series() : series_base<T, K>() {};

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;

        return !isfinite(x);
	}

	inline constexpr T calculateSum(const T& x){

        using std::log;

		return x*(static_cast<T>(4) * log(static_cast<T>(2)) - static_cast<T>(3));
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> four_ln2_m_3_series<T, K>::generateSeries(
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
    series_base<T,K>::series_name = "four_ln2_m_3_series";

	if constexpr ( std::is_same<T, float_precision> :: value ){
		series_base<T, K>::precision = x.precision();
	} else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
		series_base<T, K>::precision = std::max(x.real().precision(), x.imag().precision());
	}

	std::vector<T> vecAn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecAn[0] = static_cast<T>(-0.25)*x;
	std::vector<T> vecSn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecSn[0] = static_cast<T>(-0.25)*x;

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += minus_one_raised_to_power_n<T,K>(j+1) * x / static_cast<T>((j+1)*(j+1)*(j+2)*(j+2));
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}