#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class exp_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit exp_series(T x = static_cast<T>(0));

	virtual SeriesResult<T> generateSeries(K vecSize) const override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;

		return !isfinite(x);
	}

	inline constexpr T calculateSum(const T& x){

		using std::exp;

		return exp(x);
	}

};

template <AcceptedLike T, UnsignedIntLike K>
exp_series<T, K>::exp_series(T x) : series_base<T, K>(x)
{
	series_base<T,K>::series_name = "exp(x)";
	// Сходится при ∀x ∈ ℝ

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite");
	}

	series_base<T,K>::sum = calculateSum(x);

	if constexpr ( std::is_same<T, float_precision> :: value ){
		series_base<T, K>::precision = x.precision();
	} else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
		series_base<T, K>::precision = max(x.real().precision(), x.imag().precision());
	}
	
}

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> exp_series<T, K>::generateSeries(K vecSize) const {

	std::vector<T> vecAn(vecSize, convertArbWithPrecision<T>(0.0, series_base<T, K>::precision)); vecAn[0] = static_cast<T>(1);
	std::vector<T> vecSn(vecSize, convertArbWithPrecision<T>(0.0, series_base<T, K>::precision)); vecSn[0] = static_cast<T>(1);

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += vecAn[j-static_cast<K>(1)] * series_base<T,K>::x / static_cast<T>(j);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.an = vecAn, .Sn = vecSn};

}