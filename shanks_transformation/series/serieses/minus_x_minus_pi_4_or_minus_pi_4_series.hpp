#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class minus_x_minus_pi_4_or_minus_pi_4_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit minus_x_minus_pi_4_or_minus_pi_4_series() : series_base<T, K>("minus_x_minus_pi_4_or_minus_pi_4_series") {};

	virtual series_result<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const T& x){
		
		using std::isfinite;

		if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag()) || abs(x) >= abs(static_cast<T>(std::numbers::pi));
        } else {
		    return !isfinite(x) || abs(x) >= static_cast<T>(std::numbers::pi);
        }
	}

	inline constexpr T calculate_sum(const T& x){

		if constexpr (isComplexLike<T>::value){
            if (float_precision(x.real()) <= float_precision(0)){
                return static_cast<T>(-1) * x;
            } else {
                return static_cast<T>(0);
            }
        } else {
            if (x <= static_cast<T>(0)){
                return static_cast<T>(-1) * x;
            } else {
                return static_cast<T>(0);
            }
        }

	}

};

template<AcceptedLike T, UnsignedIntLike K>
series_result<T> minus_x_minus_pi_4_or_minus_pi_4_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or |x| >= pi");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_sum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

	vecAn[0] = static_cast<T>(std::numbers::pi * 0.25);
	vecSn[0] = static_cast<T>(std::numbers::pi * 0.25);

	using std::cos;
	using std::sin;

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += (minus_one_raised_to_power_n<T, K>(j) - static_cast<T>(1)) / (static_cast<T>(std::numbers::pi)*static_cast<T>(j*j))*cos(static_cast<T>(j)*x) +
		minus_one_raised_to_power_n<T, K>(j) / static_cast<T>(j) * sin(static_cast<T>(j) * x);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return series_result<T>{.Sn = vecSn, .an = vecAn };

}