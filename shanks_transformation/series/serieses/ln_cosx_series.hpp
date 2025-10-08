#pragma once

#include "../series_base.hpp"
#include <cmath>

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class ln_cosx_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit ln_cosx_series() : series_base<T, K>("ln_cosx_series") {};

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;

		using std::isfinite;
        if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
            return !isfinite(x) && abs(x) >= float_precision(std::numbers::pi * 0.5);
        } else {
            return !isfinite(x) && abs(x) >= static_cast<T>(std::numbers::pi * 0.5);
        }
	}

	inline constexpr T calculateSum(const T& x){

		using std::log;
		using std::cos;

		return log(cos(x));
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> ln_cosx_series<T, K>::generateSeries(
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

	if constexpr ( std::is_same<T, float_precision> :: value ){
		series_base<T, K>::precision = x.precision();
	} else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
		series_base<T, K>::precision = std::max(x.real().precision(), x.imag().precision());
	}

	using std::sin;

	const T sinx_2 = sin(x) * sin(x);

	std::vector<T> vecAn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecAn[0] = sinx_2 * static_cast<T>(-0.5);
	std::vector<T> vecSn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecSn[0] = sinx_2 * static_cast<T>(-0.5);

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += vecAn[j-static_cast<K>(1)] * sinx_2 * static_cast<T>(j) / static_cast<T>(j+1);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}