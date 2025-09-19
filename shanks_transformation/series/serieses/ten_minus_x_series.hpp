#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class ten_minus_x_series final : public TermCalculatorBase<T, K>
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
			return config.x <= static_cast<T>(5) || config.x >= static_cast<T>(15) || !isfinite(config.x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value)
			return config.x.real() <= static_cast<float_precision>(5) || config.x.real() >= static_cast<float_precision>(15) || !isfinite(config.x);
		
		return false;
	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return static_cast<T>(10) - this->x; }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	ten_minus_x_series() = delete;


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
	ten_minus_x_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
ten_minus_x_series<T, K>::ten_minus_x_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x must be in (5, 15)");
	}

	TermCalculatorBase<T,K>::series_name = "10-x";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T ten_minus_x_series<T, K>::calculateTerm(K n) const {
	if (n == 0)
        return static_cast<T>(0);

    const T pi_n = static_cast<T>(std::numbers::pi) * static_cast<T>(n);
    const T a_n = minus_one_raised_to_power_n<T,K>(n) * static_cast<T>(10) / pi_n;
    return a_n * sin(pi_n * this->x * static_cast<T>(0.2)); // (60.4) [Rows.pdf], (1 - 2 * ((n + 1) & 1)) = (-1)^{n+1}
}
