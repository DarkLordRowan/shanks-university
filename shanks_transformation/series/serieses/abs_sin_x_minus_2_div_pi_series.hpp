#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class abs_sin_x_minus_2_div_pi_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit abs_sin_x_minus_2_div_pi_series() : series_base<T, K>("abs_sin_x_minus_2_div_pi_series") {};

	virtual SeriesResult<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const T& x){
		
		using std::isfinite;
		
        if constexpr (isComplexLike<T>::value){
            return !isfinite(x.real()) || !isfinite(x.imag()) || float_precision(x.real()) < float_precision(0) || 
			float_precision(x.real()) > float_precision(2.0 * std::numbers::pi);
        } else {
            return !isfinite(x) || x < static_cast<T>(0) || x > static_cast<T>(2.0 * std::numbers::pi);
        }

	}

	inline constexpr T calculate_sum(const T& x){

		using std::sin;

		if constexpr (isComplexLike<T>::value){
            if (float_precision(x.real()) <= float_precision(std::numbers::pi)){
                return sin(x) - static_cast<T>(2) / static_cast<T>(std::numbers::pi);
            } else {
                return static_cast<T>(-1)*sin(x) - static_cast<T>(2) / static_cast<T>(std::numbers::pi);
            }

        } else {
            if (x <= static_cast<T>(std::numbers::pi)){
                return sin(x) - static_cast<T>(2) / static_cast<T>(std::numbers::pi);
            } else {
                return static_cast<T>(-1)*sin(x) - static_cast<T>(2) / static_cast<T>(std::numbers::pi);
            }
        }
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> abs_sin_x_minus_2_div_pi_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or did not in [0, 2pi]");
	}


	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_sum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

    using std::cos;

    vecAn[0] = static_cast<T>(-4) * cos(static_cast<T>(2)*x) / static_cast<T>(3.0 * std::numbers::pi);
    vecSn[0] = static_cast<T>(-4) * cos(static_cast<T>(2)*x) / static_cast<T>(3.0 * std::numbers::pi);
	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += static_cast<T>(-4) * cos(static_cast<T>(fma(2,j,2))*x) / static_cast<T>(fma(2,j,1)*fma(2,j,3)) / static_cast<T>(std::numbers::pi);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}