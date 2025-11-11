#pragma once

#include "series_base.hpp"

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

	virtual series_result<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const T& x){
		
		using std::isfinite;

        return !isfinite(x) || x < static_cast<T>(0) || x > static_cast<T>(2.0 * std::numbers::pi);
	}

	inline constexpr T calculate_sum(const T& x){

        if (x <= static_cast<T>(std::numbers::pi)){
            return static_cast<T>(std::numbers::pi) * x - x * x;
        } 

        return x * x - static_cast<T>(3.0 * std::numbers::pi) * x + static_cast<T>(2.0 * std::numbers::pi) * static_cast<T>(std::numbers::pi);
	}

};

template<AcceptedLike T, UnsignedIntLike K>
series_result<T> pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or Re(x)<=0 or Re(x)>=2pi");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_sum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

    using std::sin;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += static_cast<T>(8) * sin(static_cast<T>(fma(2,j,1)) * x) / (static_cast<T>(std::numbers::pi) * static_cast<T>(fma(2,j,1)*fma(2,j,1)*fma(2,j,1)));
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return series_result<T>{.Sn = vecSn, .an = vecAn };

}

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <FloatLike T, UnsignedIntLike K>
class pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series<complex_precision<T>, K> final : public series_base<complex_precision<T>, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series() : series_base<complex_precision<T>, K>("pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series") {};

	virtual series_result<complex_precision<T>> generate_series(
        const complex_precision<T>& x , 
		const K vecSize, 
		const complex_precision<T>& addTParameter = complex_precision<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const complex_precision<T>& x){
		
		using std::isfinite;

    	return !isfinite(x.real()) || !isfinite(x.imag()) || x.real() < static_cast<T>(0) || x.real() > static_cast<T>(2.0 * std::numbers::pi);
	}

	inline constexpr complex_precision<T> calculate_sum(const complex_precision<T>& x){

        if (x.real() <= static_cast<T>(std::numbers::pi)){
            return  complex_precision<T>(std::numbers::pi) * x - x * x;
        } 

        return x * x - complex_precision<T>(3.0 * std::numbers::pi) * x + complex_precision<T>(2.0*std::numbers::pi) * complex_precision<T>(std::numbers::pi);

	}

};

template<FloatLike T, UnsignedIntLike K>
series_result<complex_precision<T>> pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_series<complex_precision<T>, K>::generate_series(
    const complex_precision<T>& x , 
	const K vecSize, 
	const complex_precision<T>& addTParameter,
	const K addKParameter
) {

	using Complex = complex_precision<T>;

	if(check_domain(x)){
		series_base<Complex, K>::throw_domain_error("x is not finite or Re(x)<=0 or Re(x)>=2pi");
	}

	series_base<Complex,K>::x_ = x;
	series_base<Complex,K>::sum = calculate_sum(x);

	std::vector<Complex> vecAn;
	std::vector<Complex> vecSn;

	series_base<Complex,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

    using std::sin;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += Complex(8) * sin(Complex(fma(2,j,1)) * x) / (Complex(std::numbers::pi) * Complex(fma(2,j,1)*fma(2,j,1)*fma(2,j,1)));
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return series_result<Complex>{.Sn = vecSn, .an = vecAn };

}