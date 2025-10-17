#pragma once

#include "../series_base.hpp"
#include <cmath>

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class ln_x_plus_one_x_minus_one_halfed_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit ln_x_plus_one_x_minus_one_halfed_series() : series_base<T, K>("ln_x_plus_one_x_minus_one_halfed_series") {};

	virtual SeriesResult<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const T& x){
		
		using std::isfinite;

        if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag()) || abs(x) >= abs(static_cast<T>(std::numbers::pi * 0.5));
        } else {
            return !isfinite(x) || abs(x) >= static_cast<T>(std::numbers::pi * 0.5);
        }
	}

	inline constexpr T calculate_sum(const T& x){

		using std::log;
		using std::cos;

		return log(cos(x));
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> ln_x_plus_one_x_minus_one_halfed_series<T, K>::generate_series(
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

	using std::sin;

	const T sinx_2 = sin(x) * sin(x);

	vecAn[0] = sinx_2 * static_cast<T>(-0.5);
	vecSn[0] = sinx_2 * static_cast<T>(-0.5);

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += vecAn[j-static_cast<K>(1)] * sinx_2 * static_cast<T>(j) / static_cast<T>(j+1);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}