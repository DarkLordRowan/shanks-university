#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class one_minus_sqrt_1minus4x_div_2x_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit one_minus_sqrt_1minus4x_div_2x_series() : series_base<T, K>("one_minus_sqrt_1minus4x_div_2x_series") {};

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

		using std::sqrt;

		return (static_cast<T>(1) - sqrt(static_cast<T>(1) - static_cast<T>(4) * x)) / (static_cast<T>(2) * x);
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> one_minus_sqrt_1minus4x_div_2x_series<T, K>::generateSeries(
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

	std::vector<T> vecAn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecAn[0] = x;
	std::vector<T> vecSn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecSn[0] = x;

    using std::pow;
    const T x_4 = pow(x, static_cast<T>(4));

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += vecAn[j-static_cast<K>(1)] * x_4 / static_cast<T>(fma(4,j-1,5)*fma(4,j-1,4)*fma(4,j-1,3)*fma(4,j-1,2));
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

    aa
}