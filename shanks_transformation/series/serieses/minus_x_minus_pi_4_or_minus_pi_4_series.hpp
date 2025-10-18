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

		return !isfinite(x) || abs(x) >= static_cast<T>(std::numbers::pi);

	}

	inline constexpr T calculate_sum(const T& x){

        if (x <= static_cast<T>(0)){
            return static_cast<T>(-1) * x;
        } 

        return static_cast<T>(0);

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

/**
* @brief Maclaurin series of exp(x) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <FloatLike T, UnsignedIntLike K>
class minus_x_minus_pi_4_or_minus_pi_4_series<complex_precision<T>, K> final : public series_base<complex_precision<T>, K>
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Bolshakov M.P.
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @param x The argument for function series
	*/
	explicit minus_x_minus_pi_4_or_minus_pi_4_series() : series_base<complex_precision<T>, K>("minus_x_minus_pi_4_or_minus_pi_4_series") {};

	virtual series_result<complex_precision<T>> generate_series(
        const complex_precision<T>& x , 
		const K vecSize, 
		const complex_precision<T>& addTParameter = static_cast<complex_precision<T>>(1),
		const K addKParameter = static_cast<K>(1)
    ) override;

	inline constexpr bool check_domain(const complex_precision<T>& x){
		
		using std::isfinite;

		return !isfinite(x.real()) || !isfinite(x.imag()) || abs(x) >= static_cast<T>(std::numbers::pi);

	}

	inline constexpr complex_precision<T> calculate_sum(const complex_precision<T>& x){

        if (x.real() <= static_cast<T>(0)){
            return complex_precision<T>(-1) * x;
        }

        return complex_precision<T>(0);

	}

};

template<FloatLike T, UnsignedIntLike K>
series_result<complex_precision<T>> minus_x_minus_pi_4_or_minus_pi_4_series<complex_precision<T>, K>::generate_series(
    const complex_precision<T>& x , 
	const K vecSize, 
	const complex_precision<T>& addTParameter,
	const K addKParameter
) {

	using Complex = complex_precision<T>;

	if(check_domain(x)){
		series_base<Complex, K>::throw_domain_error("x is not finite or |x| >= pi");
	}

	series_base<Complex,K>::x_ = x;
	series_base<Complex,K>::sum = calculate_sum(x);

	std::vector<Complex> vecAn;
	std::vector<Complex> vecSn;

	series_base<Complex,K>::init_vecs_with_prec(vecSn,vecAn, vecSize, x);

	vecAn[0] = static_cast<Complex>(std::numbers::pi * 0.25);
	vecSn[0] = static_cast<Complex>(std::numbers::pi * 0.25);

	using std::cos;
	using std::sin;

	for(K j = static_cast<K>(1); j < vecSize; ++j){
		vecAn[j] += (minus_one_raised_to_power_n<Complex, K>(j) - Complex(1)) / (Complex(std::numbers::pi)*Complex(j*j))*cos(Complex(j)*x) +
		minus_one_raised_to_power_n<Complex, K>(j) / Complex(j) * sin(Complex(j) * x);
		vecSn[j] += vecSn[j-static_cast<K>(1)] + vecAn[j];
	}

	return series_result<Complex>{.Sn = vecSn, .an = vecAn };

}