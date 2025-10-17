#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class x_min_sqrt_x_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit x_min_sqrt_x_series() : series_base<T, K>("x_min_sqrt_x_series") {};

	virtual SeriesResult<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const T& x){
		
		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag()) || float_precision(abs(x - static_cast<T>(1))) >= float_precision(1);
        } else {
		    return !isfinite(x) || abs(x - static_cast<T>(1)) >= static_cast<T>(1);
        }
	}

	inline constexpr T calculate_sum(const T& x){

		using std::sqrt;

		return x - sqrt(x);
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> x_min_sqrt_x_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or x<=0 or x>=2");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_sum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

	vecAn[0] = static_cast<T>(-1);
	vecSn[0] = x - static_cast<T>(1);

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] -= vecAn[j-static_cast<K>(1)] * (x - static_cast<T>(1)) * fma(static_cast<T>(2), static_cast<T>(j-1), static_cast<T>(-1)) / static_cast<T>(2 * j);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	vecAn[0]+=x;

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}