#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class cos3xmin1_div_xsquare_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit cos3xmin1_div_xsquare_series() : series_base<T, K>("cos3xmin1_div_xsquare_series") {};

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
        return !isfinite(x) || x==static_cast<T>(0);

	}

	inline constexpr T calculateSum(const T& x){

		using std::cos;

		return cos(static_cast<T>(3)*x-static_cast<T>(1)) / (x * x);
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> cos3xmin1_div_xsquare_series<T, K>::generateSeries(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or x = 0");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculateSum(x);

	if constexpr ( std::is_same<T, float_precision> :: value ){
		series_base<T, K>::precision = x.precision();
	} else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
		series_base<T, K>::precision = std::max(x.real().precision(), x.imag().precision());
	}

	std::vector<T> vecAn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecAn[0] = static_cast<T>(1) / (x * x);
	std::vector<T> vecSn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecSn[0] = static_cast<T>(1) / (x * x);

    using std::pow;

	const T var_3x_1squared = fma(static_cast<T>(3),x,static_cast<T>(-1)) * fma(static_cast<T>(3),x,static_cast<T>(-1));
	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += static_cast<T>(-1) * vecAn[j-static_cast<K>(1)] * var_3x_1squared / static_cast<T>(2*j*fma(2,j-1,1));
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}