#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class x_1mx_squared_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit x_1mx_squared_series() : series_base<T, K>("x_1mx_squared_series") {};

	virtual SeriesResult<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const T& x){
		
		using std::isfinite;
		if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag()) && float_precision(abs(x)) >= float_precision(1);
        } else {
		    return !isfinite(x) && abs(x) >= static_cast<T>(1);
        }
	}

	inline constexpr T calculate_sum(const T& x){

		using std::sqrt;

		return x / ((static_cast<T>(1) - x) * (static_cast<T>(1) - x));
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> x_1mx_squared_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or |x|>=1");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_sum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

	vecAn[0] = x;
	vecSn[0] = x;

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += vecAn[j-static_cast<K>(1)] * x * static_cast<T>(j + 1) / static_cast<T>(j);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}