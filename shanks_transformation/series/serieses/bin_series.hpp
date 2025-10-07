#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class bin_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit bin_series() : series_base<T, K>() {};

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;

        if constexpr(std::is_same<T, complex_precision<float_precision>>::value){

            return !isfinite(x) && abs(x) >= float_precision(1);

        } else {
		    return !isfinite(x) && abs(x) >= static_cast<T>(1);
        }
	}

	inline constexpr T calculateSum(const T& x, const T& alpha){

		using std::pow;

		return pow(static_cast<T>(1) + x, alpha);
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> bin_series<T, K>::generateSeries(
    const T& x, 
	const K vecSize, 
	const T& addTParameter, //alpha
	const K addKParameter
) {

    using std::to_string;

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculateSum(x, addTParameter); 
    series_base<T,K>::series_name = "bin_series " + to_string(addTParameter);

	if constexpr ( std::is_same<T, float_precision> :: value ){
		series_base<T, K>::precision = x.precision();
	} else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
		series_base<T, K>::precision = std::max(x.real().precision(), x.imag().precision());
	}

	std::vector<T> vecAn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecAn[0] = static_cast<T>(1);
	std::vector<T> vecSn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision)); vecSn[0] = static_cast<T>(1);

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += vecAn[j-static_cast<K>(1)] * (addTParameter - static_cast<T>(j - static_cast<K>(1))) * x / static_cast<T>(j);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}