#pragma once

#include "../series_base.hpp"
#include <cstdlib>

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class minus_3_div_4_or_x_minus_3_div_4_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit minus_3_div_4_or_x_minus_3_div_4_series() : series_base<T, K>("minus_3_div_4_or_x_minus_3_div_4_series") {};

	virtual SeriesResult<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const T& x){
		
		using std::isfinite;
		
        if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag()) || abs(x) >= abs(static_cast<T>(3)); 
        } else {
            return !isfinite(x) || abs(x) >= static_cast<T>(3);
        }

	}

	inline constexpr T calculate_sum(const T& x){

		if constexpr (isComplexLike<T>::value){
            if (float_precision(x.real()) <= float_precision(0)){
                return static_cast<T>(-0.75);
            } else {
                return x - static_cast<T>(-0.75);
            }

        } else {
            if (x <= static_cast<T>(0)){
                return static_cast<T>(-0.75);
            } else {
                return x - static_cast<T>(0.75);
            }
        }
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> minus_3_div_4_or_x_minus_3_div_4_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or |x|>=3");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_sum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

	const T piDiv3 = static_cast<T>(std::numbers::pi) / static_cast<T>(3);

	using std::cos;
	using std::sin;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += static_cast<T>(-2) / (piDiv3 * piDiv3 * static_cast<T>(3 * fma(2,j,1)*fma(2,j,1)))*cos(static_cast<T>(j+1)*piDiv3*x)+
		minus_one_raised_to_power_n<T, K>(j) / (piDiv3 * static_cast<T>(j+1)) * sin(static_cast<T>(j+1) * piDiv3 * x);
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}