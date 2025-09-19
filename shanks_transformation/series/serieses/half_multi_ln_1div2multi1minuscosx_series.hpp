#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class half_multi_ln_1div2multi1minuscosx_series final : public TermCalculatorBase<T, K>
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
			return config.x <= static_cast<T>(0) || config.x >= static_cast<T>(2.0 * PI) || !isfinite(config.x) || cos(config.x) == static_cast<T>(1); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return config.x.real() <= static_cast<float_precision>(0) || config.x.real() >= static_cast<float_precision>(2.0) * arbPI || 
			!isfinite(config.x) || cos(config.x) == static_cast<T>(1); 
		
		return false;

	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return static_cast<T>(0.5) * log(static_cast<T>(0.5) / (static_cast<T>(1) -  cos(this->x))); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	half_multi_ln_1div2multi1minuscosx_series() = delete;


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
	half_multi_ln_1div2multi1minuscosx_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
half_multi_ln_1div2multi1minuscosx_series<T, K>::half_multi_ln_1div2multi1minuscosx_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x must be in (0, 2π) and cos(x) ≠ 1");
	}

	TermCalculatorBase<T,K>::series_name = "0.5*ln(1/(2*(1-cos(x))))";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T half_multi_ln_1div2multi1minuscosx_series<T, K>::calculateTerm(K n) const {
	const T a = static_cast<T>(n + 1);
    return cos(this->x * a) / a; // (47.5) [Rows.pdf]
}