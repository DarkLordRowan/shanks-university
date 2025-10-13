#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series() : series_base<T, K>("pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series") {};

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;
        if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag()) || abs(x) >= abs(static_cast<T>(std::numbers::pi));
        } else {
            return !isfinite(x) || abs(x) >= static_cast<T>(std::numbers::pi);;
        }


	}

	inline constexpr T calculateSum(const T& x){

		if constexpr (isComplexLike<T>::value){
            if (float_precision(x.real()) <= float_precision(0)){
                return static_cast<T>(0.25 * std::numbers::pi);
            } else {
                return static_cast<T>(0.25 * std::numbers::pi) - x;
            }

        } else { 
            if (x <= static_cast<T>(0)){
                return static_cast<T>(0.25 * std::numbers::pi);
            } else {
                return static_cast<T>(0.25 * std::numbers::pi) - x;
            }
        }
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>::generateSeries(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or |x|>pi");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculateSum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::initVecsWithPrec(vecSn,vecAn, vecSize, x);

    using std::sin;
	using std::cos;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += (static_cast<T>(1) + minus_one_raised_to_power_n<T, K>(j)) / (static_cast<T>(std::numbers::pi) * static_cast<T>((j+1) * (j+1))) * cos(static_cast<T>(j+1) * x) +
		minus_one_raised_to_power_n<T,K>(j+1) / static_cast<T>(j+1) * sin(static_cast<T>(j+1) * x);
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}