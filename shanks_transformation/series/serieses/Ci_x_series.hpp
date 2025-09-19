#pragma once
#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of function Ci(x)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class Ci_x_series final : public TermCalculatorBase<T, K>
{
protected:

    const T gamma = static_cast<T>(std::numbers::egamma); // the Euler–Mascheroni constant

    /**
     * @brief 
     * 
     * @param x 
     * @return true 
     * @return false 
     */
    inline bool domain_checker(const SeriesConfig<T,K>& config) const{  return config.x == static_cast<T>(0) || !isfinite(config.x); }

    /**
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return static_cast<T>(0); }

public:

    /**
    * @brief Construct a new Ci_x_series object
    * 
    */
    Ci_x_series() = delete;

    /**
    * @brief Computes the nth term of the integral cos series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the integral cos series
    */
    [[nodiscard]] constexpr virtual T calculateTerm(K n) const override;

    /**
	 * @brief 
	 * 
	 * @param config 
	 */
	Ci_x_series(const SeriesConfig<T,K>& config);

};

template <Accepted T, std::unsigned_integral K>
Ci_x_series<T, K>::Ci_x_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x must be finite and non-zero");
	}

	TermCalculatorBase<T,K>::series_name = "Ci(x)";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T Ci_x_series<T, K>::calculateTerm(K n) const {
    if (n == 0)
        return gamma + log(this->x); // (65.1) [Rows.pdf]

    const T two_n = static_cast<T>(2 * n);
    return minus_one_raised_to_power_n<T,K>(n) * pow(this->x, two_n) / two_n / static_cast<T>(fact<K>(2 * n)); // (65.1) [Rows.pdf], (1 - 2 * (n & 1)) = (-1)^{n}
}
