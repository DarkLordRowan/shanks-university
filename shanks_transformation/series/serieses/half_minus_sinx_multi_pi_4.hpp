#pragma once

#include "../series_base.hpp"
#include <cmath>

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class half_minus_sinx_multi_pi_4 final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit half_minus_sinx_multi_pi_4() : series_base<T, K>("half_minus_sinx_multi_pi_4") {};

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;

        if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
            return !isfinite(x) && (x.real() < float_precision(0)) && (x.real() > float_precision(0.5 * std::numbers::pi));
        } else {
            return !isfinite(x) && (x < static_cast<T>(0)) && (x > static_cast<T>(0.5 * std::numbers::pi));
        }
	}

	inline constexpr T calculateSum(const T& x){

		using std::sin;

		return static_cast<T>(0.5) - static_cast<T>(std::numbers::pi * 0.25) * sin(x);
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> half_minus_sinx_multi_pi_4<T, K>::generateSeries(
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

	std::vector<T> vecAn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision));
	std::vector<T> vecSn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision));

    using std::cos;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += cos(static_cast<T>(fma(2,j,2)) * x) / static_cast<T>(fma(2,j,1) * fma(2,j,3));
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}