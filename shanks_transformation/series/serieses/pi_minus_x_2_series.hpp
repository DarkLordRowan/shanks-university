#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class pi_minus_x_2_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit pi_minus_x_2_series() : series_base<T, K>("pi_minus_x_2_series") {};

	virtual SeriesResult<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const T& x){
		
		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag()) || float_precision(x.real()) <= float_precision(0) || 
			float_precision(x.real()) >= float_precision(std::numbers::pi);
        } else {
            return !isfinite(x) || x <= static_cast<T>(0) || x >= static_cast<T>(std::numbers::pi);
        }
	}

	inline constexpr T calculate_sum(const T& x){

		return (static_cast<T>(std::numbers::pi) - x) * static_cast<T>(0.5);
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> pi_minus_x_2_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or Re(x)<=0 or Re(x)>=pi");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_sum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

    using std::sin;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += sin(static_cast<T>(j+1)*x) / static_cast<T>(j+1);
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };
}