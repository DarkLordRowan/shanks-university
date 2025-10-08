#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class arcsin_x_minus_x final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit arcsin_x_minus_x() : series_base<T, K>() {};

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;
        if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
            return !isfinite(x) && abs(x) >= float_precision(1);
        } else {
            return !isfinite(x) && abs(x) >= static_cast<T>(1);
        }


	}

	inline constexpr T calculateSum(const T& x){

		using std::asin;

        return asin(x)-x;

	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> arcsin_x_minus_x<T, K>::generateSeries(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite");
	}

    if constexpr ( std::is_same<T, float_precision> :: value ){
		series_base<T, K>::precision = x.precision();
	} else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
		series_base<T, K>::precision = std::max(x.real().precision(), x.imag().precision());
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculateSum(x);
    series_base<T,K>::series_name = "arcsin_x_minus_x";

    using std::pow;

	std::vector<T> vecAn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecAn[0] = pow(x,static_cast<T>(3)) / static_cast<T>(6);
	std::vector<T> vecSn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecSn[0] = pow(x,static_cast<T>(3)) / static_cast<T>(6);


	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += vecAn[j-1] * x * x * static_cast<T>(fma(2,j,1)*fma(2,j,1)) / static_cast<T>(fma(2,j,2)*fma(2,j,3));
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}