#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series() : series_base<T, K>("pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series") {};

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

		return 0;
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>::generateSeries(
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

	std::vector<T> vecAn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecAn[0] = static_cast<T>(-std::numbers::pi * 0.125);
	std::vector<T> vecSn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecSn[0] = static_cast<T>(-std::numbers::pi * 0.125);

    using std::sin;
	using std::cos;

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += (static_cast<T>(1) + minus_one_raised_to_power_n<T, K>(j+1)) / (static_cast<T>(std::numbers::pi) * static_cast<T>(j * j)) * cos(static_cast<T>(j) * x) +
		minus_one_raised_to_power_n<T,K>(j) / static_cast<T>(j) * sin(static_cast<T>(j) * x);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}