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

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;
		
        if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
            return !isfinite(x) || abs(x) >= float_precision(3); 
        } else {
            return !isfinite(x) || abs(x) >= static_cast<T>(3);
        }

	}

	inline constexpr T calculateSum(const T& x){

        using Complex = complex_precision<float_precision>;

		if constexpr (std::is_same<T, Complex>::value){
            if (x.real() <= float_precision(0)){
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
SeriesResult<T> minus_3_div_4_or_x_minus_3_div_4_series<T, K>::generateSeries(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or |x|>=3");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculateSum(x);

	if constexpr ( std::is_same<T, float_precision> :: value ){
		series_base<T, K>::precision = x.precision();
	} else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
		series_base<T, K>::precision = std::max(x.real().precision(), x.imag().precision());
	}

	std::vector<T> vecAn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision));
	std::vector<T> vecSn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision));

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