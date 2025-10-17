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
	explicit exp_series() : series_base<T, K>("exp_series") {};

	virtual series_result<T> generate_series(
		const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
	) override;

	inline constexpr bool check_domainn(const T& x) const{
		
		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag());
		} else {
			return !isfinite(x);
		}
	}

	inline constexpr T calculate_summ(const T& x) const {

		using std::exp;

		return exp(x);
	}

};

template<AcceptedLike T, UnsignedIntLike K>
series_result<T> exp_series<T, K>::generate_series(
	const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter 
) {

	if(check_domainn(x)){
		series_base<T, K>::throw_domain_error("x is not finite");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_summ(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

	vecAn[0] = static_cast<T>(1);
	vecSn[0] = static_cast<T>(1);

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += vecAn[j-static_cast<K>(1)] * x / static_cast<T>(j);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return series_result<T>{.Sn = vecSn, .an = vecAn };

}