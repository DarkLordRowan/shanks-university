#pragma once

#include "../series_base.hpp"
#include <type_traits>

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class riemann_zeta_func_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit riemann_zeta_func_series() : series_base<T, K>("riemann_zeta_func_series") {};

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;

		if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
            return !isfinite(x) || x.real() < float_precision(1);
        } else {
            return !isfinite(x) || x < static_cast<T>(1);
        }
	}

	inline T calculateSum(const T& x){

		if constexpr(std::is_floating_point<T>::value || std::is_same<T, float_precision>::value){
			const float_precision value = float_precision(x, series_base<T,K>::precision);
			return static_cast<T>(zeta(value));
		} else {
			return 0;  aa
		}

	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> riemann_zeta_func_series<T, K>::generateSeries(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or Re(x)<1");
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

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += pow(static_cast<T>(j+1), -x);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}