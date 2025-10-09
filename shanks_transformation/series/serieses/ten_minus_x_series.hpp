#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class ten_minus_x_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit ten_minus_x_series() : series_base<T, K>("ten_minus_x_series") {};

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){

		using std::isfinite;

		if constexpr (std::is_same<T, complex_precision<float_precision>> :: value){
			return !isfinite(x) || abs(x - static_cast<T>(10)) > float_precision(5, series_base<T,K>::precision);
		} else {
			return !isfinite(x) || abs(x - static_cast<T>(10)) > static_cast<T>(5);
		}
	}

	inline constexpr T calculateSum(const T& x){

		return static_cast<T>(10) - x;
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> ten_minus_x_series<T, K>::generateSeries(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or x <=5 or x >=15");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculateSum(x);

	if constexpr ( std::is_same<T, float_precision> :: value ){
		series_base<T, K>::precision = x.precision();
	} else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
		series_base<T, K>::precision = std::max(x.real().precision(), x.imag().precision());
	}

	std::vector<T> vecAn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision));;
	std::vector<T> vecSn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision));

	using std::sin;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += static_cast<T>(10) * minus_one_raised_to_power_n<T, K>(j+1) / (static_cast<T>(j+1) * static_cast<T>(std::numbers::pi)) * 
		sin(static_cast<T>(j+1) * static_cast<T>(std::numbers::pi * 0.2) * x);
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}