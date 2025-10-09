#pragma once

#include "../series_base.hpp"

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, UnsignedIntLike K>
class pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series final : public series_base<T, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series() : series_base<T, K>("pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series") {};

	virtual SeriesResult<T> generateSeries(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool checkDomain(const T& x){
		
		using std::isfinite;

		if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
            return !isfinite(x) || x.real() < float_precision(0) || x.real() > float_precision(2.0 * std::numbers::pi);
        } else {
            return !isfinite(x) || x < static_cast<T>(0) || x > static_cast<T>(2.0 * std::numbers::pi);
        }
	}

	inline constexpr T calculateSum(const T& x){

		if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
            if (x.real() <= float_precision(std::numbers::pi)){
                return  static_cast<T>(float_precision(std::numbers::pi, series_base<T, K>::precision)) * x - x * x;
            } else {
                return x * x - static_cast<T>(3 * float_precision(std::numbers::pi, series_base<T, K>::precision)) * x 
				+ static_cast<T>(2 * float_precision(std::numbers::pi, series_base<T, K>::precision)) *
				 static_cast<T>(float_precision(std::numbers::pi, series_base<T, K>::precision));
            }

        } else {
            if (x <= static_cast<T>(0)){
                return static_cast<T>(std::numbers::pi) * x - x * x;
            } else {
                return x * x - static_cast<T>(3.0 * std::numbers::pi) * x + static_cast<T>(2.0 * std::numbers::pi) * static_cast<T>(std::numbers::pi);
            }
        }
	}

};

template<AcceptedLike T, UnsignedIntLike K>
SeriesResult<T> pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series<T, K>::generateSeries(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(checkDomain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or Re(x)<=0 or Re(x)>=2pi");
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

    using std::sin;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += static_cast<T>(8) * sin(static_cast<T>(fma(2,j,1)) * x) / (static_cast<T>(std::numbers::pi) * static_cast<T>(fma(2,j,1)*fma(2,j,1)*fma(2,j,1)));
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return SeriesResult<T>{.Sn = vecSn, .an = vecAn };

}