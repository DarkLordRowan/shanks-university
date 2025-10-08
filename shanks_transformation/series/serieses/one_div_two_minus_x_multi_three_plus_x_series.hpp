#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class one_div_two_minus_x_multi_three_plus_x_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit one_div_two_minus_x_multi_three_plus_x_series() : series_base<T, K>("one_div_two_minus_x_multi_three_plus_x_series") {};

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


		return static_cast<T>(1) / ((static_cast<T>(2) - x) * (static_cast<T>(3) + x));
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> one_div_two_minus_x_multi_three_plus_x_series<T, K>::generateSeries(
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

    using std::pow;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += pow(x, static_cast<T>(j)) * static_cast<T>(0.2) * minus_one_raised_to_power_n<T, K>(j) * static_cast<T>(pow(2, j+1) + pow(3, j + 1)) / static_cast<T>(pow(6, j+1));
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}