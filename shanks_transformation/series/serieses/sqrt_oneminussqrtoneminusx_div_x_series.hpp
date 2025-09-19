#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class sqrt_oneminussqrtoneminusx_div_x_series final : public TermCalculatorBase<T, K>
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
			return abs(config.x) >= static_cast<T>(1) || config.x == static_cast<T>(0) || !isfinite(config.x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value)
			return abs(config.x) >= static_cast<float_precision>(1) || config.x == static_cast<T>(0) || !isfinite(config.x);
		
		return false;
	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return sqrt((static_cast<T>(1) - sqrt(static_cast<T>(1) - this->x)) / this->x); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	sqrt_oneminussqrtoneminusx_div_x_series() = delete;


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
	sqrt_oneminussqrtoneminusx_div_x_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
sqrt_oneminussqrtoneminusx_div_x_series<T, K>::sqrt_oneminussqrtoneminusx_div_x_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be in (0, 1)");
	}

	TermCalculatorBase<T,K>::series_name = "sqrt((1-sqrt(1-x))/x)";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T sqrt_oneminussqrtoneminusx_div_x_series<T, K>::calculateTerm(K n) const {
	const K a = 2 * n;
    const K b = 2 * a;
    return static_cast<T>(fact<K>(b)) * pow(this->x, static_cast<T>(n)) / 
    (sqrt(static_cast<T>(2)) *  static_cast<T>(pow(2, b) * fact<K>(a) * fact<K>(a + 1))); // (53.1) [Rows.pdf]
}
