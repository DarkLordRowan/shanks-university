#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class two_arcsin_square_x_halfed_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit two_arcsin_square_x_halfed_series() : series_base<T, K>("two_arcsin_square_x_halfed_series") {};

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag()) || float_precision(abs(x)) > float_precision(2);
        } else {
		    return !isfinite(x) || abs(x) > static_cast<T>(2);
        }
	}

	inline constexpr T calculateSum(const T& x){

		using std::asin;

		return static_cast<T>(2) * asin(x * static_cast<T>(0.5)) * asin(x * static_cast<T>(0.5));
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> two_arcsin_square_x_halfed_series<T, K>::generateSeries(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or |x|>2");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculateSum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::initVecsWithPrec(vecSn,vecAn, vecSize, x);

	vecAn[0] = x * x * static_cast<T>(0.5);
	vecSn[0] = x * x * static_cast<T>(0.5);

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += vecAn[j-static_cast<K>(1)] * x * x * static_cast<T>(j*j) / static_cast<T>(fma(2,j,1) * fma(2,j,2));
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}