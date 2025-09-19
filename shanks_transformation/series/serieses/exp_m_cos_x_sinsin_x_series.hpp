#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class exp_m_cos_x_sinsin_x_series final : public TermCalculatorBase<T, K>
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
			return abs(config.x) >= static_cast<T>(PI) || !isfinite(config.x); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(config.x.real()) >= arbPI || !isfinite(config.x); 
		
		return false;

	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return exp(-cos(this->x))*sin(sin(this->x)); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	exp_m_cos_x_sinsin_x_series() = delete;


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
	exp_m_cos_x_sinsin_x_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
exp_m_cos_x_sinsin_x_series<T, K>::exp_m_cos_x_sinsin_x_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be ≤ π for numerical stability");
	}

	TermCalculatorBase<T,K>::series_name = "exp(-cos(x))*sin(sin(x))";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T exp_m_cos_x_sinsin_x_series<T, K>::calculateTerm(K n) const {
	return minus_one_raised_to_power_n<T,K>(n + static_cast<K>(1)) * sin(static_cast<T>(n) * this->x) / static_cast<T>(fact<K>(n)); // (31.1) [Rows.pdf]
}