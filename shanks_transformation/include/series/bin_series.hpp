#pragma once

#include "series_base.hpp"

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
	explicit bin_series() : series_base<T, K>("bin_series") {};

	virtual series_result<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domainn(const T& x){
		
		using std::isfinite;

        if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag()) || abs(x) >= abs(static_cast<T>(1));

        } else {
		    return !isfinite(x) || abs(x) >= static_cast<T>(1);
        }
	}

	inline constexpr T calculate_sum(const T& x, const T& alpha){

		using std::pow;

		return pow(static_cast<T>(1) + x, alpha);
	}

};

template<AcceptedLike T, UnsignedIntLike K>
series_result<T> bin_series<T, K>::generate_series(
    const T& x, 
	const K vecSize, 
	const T& addTParameter, //alpha
	const K addKParameter
) {

    using std::to_string;

	if(check_domainn(x)){
		series_base<T, K>::throw_domain_error("x is not finite or |x|>=1");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_sum(x, addTParameter); 

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

	vecAn[0] = static_cast<T>(1);
	vecSn[0] = static_cast<T>(1);

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += vecAn[j-static_cast<K>(1)] * (addTParameter - static_cast<T>(j - static_cast<K>(1))) * x / static_cast<T>(j);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return series_result<T>{.Sn = vecSn, .an = vecAn };

}