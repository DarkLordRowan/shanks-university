#pragma once

#include "series_base.hpp"

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

	virtual series_result<T> generate_series(
        const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const T& x){
		
		using std::isfinite;

        return !isfinite(x) || abs(x) >= static_cast<T>(std::numbers::pi);


	}

	inline constexpr T calculate_sum(const T& x){

        if (x <= static_cast<T>(0)){
            return static_cast<T>(0.25 * std::numbers::pi);
        } 
		
        return static_cast<T>(0.25 * std::numbers::pi) - x;
	}

};

template<AcceptedLike T, UnsignedIntLike K>
series_result<T> pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>::generate_series(
    const T& x , 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
) {

	if(check_domain(x)){
		series_base<T, K>::throw_domain_error("x is not finite or |x|>pi");
	}

	series_base<T,K>::x_ = x;
	series_base<T,K>::sum = calculate_sum(x);

	std::vector<T> vecAn;
	std::vector<T> vecSn;

	series_base<T,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

    using std::sin;
	using std::cos;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += (static_cast<T>(1) + utils::minus_one_raised_to_power_n<T, K>(j)) / (static_cast<T>(std::numbers::pi) * static_cast<T>((j+1) * (j+1))) * cos(static_cast<T>(j+1) * x) +
		utils::minus_one_raised_to_power_n<T,K>(j+1) / static_cast<T>(j+1) * sin(static_cast<T>(j+1) * x);
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
class pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<complex_precision<T>,K> final : public series_base<complex_precision<T>, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series() : series_base<complex_precision<T>, K>("pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series") {};

	virtual series_result<complex_precision<T>> generate_series(
        const complex_precision<T>& x , 
		const K vecSize, 
		const complex_precision<T>& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const complex_precision<T>& x){
		
		using std::isfinite;
    	return !isfinite(x.real()) || !isfinite(x.imag()) || abs(x) >= static_cast<T>(std::numbers::pi);

	}

	inline constexpr complex_precision<T> calculate_sum(const complex_precision<T>& x){

        if (x.real() <= static_cast<T>(0)){
            return complex_precision<T>(0.25 * std::numbers::pi);
        } 
        return complex_precision<T>(0.25 * std::numbers::pi) - x;

	}

};

template<FloatLike T, UnsignedIntLike K>
series_result<complex_precision<T>> pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<complex_precision<T>, K>::generate_series(
    const complex_precision<T>& x , 
	const K vecSize, 
	const complex_precision<T>& addTParameter,
	const K addKParameter
) {

	using Complex = complex_precision<T>;

	if(check_domain(x)){
		series_base<Complex, K>::throw_domain_error("x is not finite or |x|>pi");
	}

	series_base<Complex,K>::x_ = x;
	series_base<Complex,K>::sum = calculate_sum(x);

	std::vector<Complex> vecAn;
	std::vector<Complex> vecSn;

	series_base<Complex,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

    using std::sin;
	using std::cos;

	for(K j = static_cast<K>(0); j < vecSize; ++j){
		vecAn[j] += (Complex(1) + utils::minus_one_raised_to_power_n<Complex, K>(j)) / (Complex(std::numbers::pi) * Complex((j+1) * (j+1))) * cos(Complex(j+1) * x) +
		utils::minus_one_raised_to_power_n<Complex,K>(j+1) / Complex(j+1) * sin(Complex(j+1) * x);
		vecSn[j] += vecSn[j == static_cast<K>(0) ? j : j-static_cast<K>(1)] + vecAn[j];
	}

	return series_result<Complex>{.Sn = vecSn, .an = vecAn };

}