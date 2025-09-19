#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class ln_1plussqrt1plusxsquare_minus_ln_2_series final : public TermCalculatorBase<T, K>
{
protected:

    /**
     * @brief 
     * 
     * @param x 
     * @return true 
     * @return false 
     */
    inline bool domain_checker(const SeriesConfig<T,K>& config) const { 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return abs(config.x) > static_cast<T>(1) || !isfinite(config.x); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(config.x) > static_cast<float_precision>(1) || !isfinite(config.x); ; 
		
		return false;
	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return log(static_cast<T>(0.5) * (static_cast<T>(1) + hypot(static_cast<T>(1), this->x))); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	ln_1plussqrt1plusxsquare_minus_ln_2_series() = delete;


	/**
	* @brief Computes the nth term of the Maclaurin series of the cosine function
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the cosine functions
	*/
	[[nodiscard]] constexpr virtual T calculateTerm(K n) const override;

	/**
	 * @brief 
	 * 
	 * @param config 
	 */
	ln_1plussqrt1plusxsquare_minus_ln_2_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
ln_1plussqrt1plusxsquare_minus_ln_2_series<T, K>::ln_1plussqrt1plusxsquare_minus_ln_2_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be ≤ 1");
	}

	TermCalculatorBase<T,K>::series_name = "ln(1+sqrt(1+x^2))-ln(2)";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T ln_1plussqrt1plusxsquare_minus_ln_2_series<T, K>::calculateTerm(K n) const {
	const K a = n + 1;
	const K b = static_cast<K>(fma(2, n, 2));
	return minus_one_raised_to_power_n<T,K>(n) * static_cast<T>(fact<K>(b - 1)) * pow(this->x, static_cast<T>(b)) /
		static_cast<T>(pow(static_cast<K>(2), b) * fact<K>(a) * fact(a)); // (49.3) [Rows.pdf]
}