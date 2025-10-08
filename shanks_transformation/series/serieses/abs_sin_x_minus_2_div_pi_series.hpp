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

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;
        if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
            return !isfinite(x) && (x.real() < float_precision(0) || x.real() > float_precision(2 * std::numbers::pi));
        } else {
            return !isfinite(x) && (x < static_cast<T>(0) || x > static_cast<T>(2 * std::numbers::pi));
        }


	}

	inline constexpr T calculateSum(const T& x){

		using std::sin;

        using Complex = complex_precision<float_precision>;

		if constexpr (std::is_same<T, Complex>::value){
            if (x.real() <= float_precision(std::numbers::pi)){
                return sin(x) - Complex(float_precision(2, series_base<T, K>::precision)) / Complex(float_precision(std::numbers::pi, series_base<T, K>::precision));
            } else {
                return Complex(-1)*sin(x) - Complex(float_precision(2, series_base<T, K>::precision)) / Complex(float_precision(std::numbers::pi, series_base<T, K>::precision));
            }

        } else  if constexpr (std::is_same<T, float_precision>::value){
            if (x <= float_precision(std::numbers::pi)){
                return sin(x) - float_precision(2, series_base<T, K>::precision) / float_precision(std::numbers::pi, series_base<T, K>::precision);
            } else {
                return float_precision(-1) * sin(x) - float_precision(2, series_base<T, K>::precision) / float_precision(std::numbers::pi, series_base<T, K>::precision);
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
SeriesResult<T> abs_sin_x_minus_2_div_pi_series<T, K>::generateSeries(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite");
	}

    if constexpr ( std::is_same<T, float_precision> :: value ){
		series_base<T, K>::precision = x.precision();
	} else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
		series_base<T, K>::precision = std::max(x.real().precision(), x.imag().precision());
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculateSum(x);

	std::vector<T> vecAn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision));
	std::vector<T> vecSn(vecSize, convertWithPrec<T>(0.0, series_base<T, K>::precision));

    using std::cos;

    vecAn[0] = static_cast<T>(-4) * cos(static_cast<T>(2)*x) / static_cast<T>(3.0 * std::numbers::pi);
    vecSn[0] = static_cast<T>(-4) * cos(static_cast<T>(2)*x) / static_cast<T>(3.0 * std::numbers::pi);
	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += static_cast<T>(-4) * cos(static_cast<T>(fma(2,j,2))*x) / static_cast<T>(fma(2,j,1)*fma(2,j,3)) / static_cast<T>(std::numbers::pi);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}