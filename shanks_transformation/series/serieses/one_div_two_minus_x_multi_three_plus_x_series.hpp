#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class one_div_two_minus_x_multi_three_plus_x_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit one_div_two_minus_x_multi_three_plus_x_series() : series_base<T, K>("one_div_two_minus_x_multi_three_plus_x_series") {};

	virtual SeriesResult<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const T& x){
		
		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag()) || abs(x) >= abs(static_cast<T>(2));
        } else {
		    return !isfinite(x) || abs(x) >= static_cast<T>(2);
        }
	}

	inline constexpr T calculate_summ(const T& x){


		return static_cast<T>(1) / ((static_cast<T>(2) - x) * (static_cast<T>(3) + x));
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> one_div_two_minus_x_multi_three_plus_x_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or |x|>=2");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_summ(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

    using std::pow;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += pow(x, static_cast<T>(j)) * static_cast<T>(0.2) * 
		(minus_one_raised_to_power_n<T, K>(j) * static_cast<T>(pow(2, j + 1)) + static_cast<T>(pow(3, j + 1))) / static_cast<T>(pow(6, j+1));
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}