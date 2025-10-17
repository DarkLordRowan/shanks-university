#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class five_pi_twelve_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit five_pi_twelve_series() : series_base<T, K>("five_pi_twelve_series") {};

	virtual SeriesResult<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domainn(const T& x){
		
		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag());
		} else {
			return !isfinite(x);
		}
	}

	inline constexpr T calculate_sum(const T& x){


		return x * static_cast<T>(std::numbers::pi * 5) / static_cast<T>(12);
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> five_pi_twelve_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domainn(x)){
		series_base<T, K>::throw_domain_error("x is not finite");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_sum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

	vecAn[0] = static_cast<T>(-5) * x / static_cast<T>(3);
	vecSn[0] = static_cast<T>(-5) * x / static_cast<T>(3);

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += minus_one_raised_to_power_n<T,K>(j % 3) * x / static_cast<T>(fma(2,j,1));
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}