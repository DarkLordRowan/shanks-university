#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class arctg_x3_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit arctg_x3_series() : series_base<T, K>("arctg_x3_series") {};

	virtual series_result<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domainn(const T& x){
		
		using std::isfinite;
        if constexpr (isComplexLike<T>::value){
    		return !isfinite(x.real()) || !isfinite(x.imag()) || abs(x) > abs(static_cast<T>(1));
        } else {
            return !isfinite(x) || abs(x) > static_cast<T>(1);
        }

	}

	inline constexpr T calculate_sum(const T& x){

		using std::atan;

        return atan(x*x*x);
	}

};

template<AcceptedLike T, UnsignedIntLike K>
series_result<T> arctg_x3_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domainn(x)){
		series_base<T, K>::throw_domain_error("x is not finite or |x|>1");
	}
	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_sum(x);

    using std::pow;

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

	vecAn[0] = pow(x, static_cast<T>(3));
	vecSn[0] = pow(x, static_cast<T>(3));

    const T x_6 = pow(x, static_cast<T>(6));
	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += static_cast<T>(-1) * vecAn[j-1] * x_6 * static_cast<T>(fma(2,j-1,1)) / static_cast<T>(fma(2,j,1));
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return series_result<T>{.Sn = vecSn, .an = vecAn };

}