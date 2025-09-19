#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class m_fact_1mx_mp1_inverse_series final : public TermCalculatorBase<T, K>
{
protected:

	K m;

    /**
     * @brief 
     * 
     * @param x 
     * @return true 
     * @return false 
     */
    inline bool domain_checker(const SeriesConfig<T,K>& config) const { 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return abs(config.x) >= static_cast<T>(1) || !isfinite(config.x); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(config.x) >= static_cast<float_precision>(1) || !isfinite(config.x); 
		
		return false;
	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return static_cast<T>(fact<K>(m)) / pow(static_cast<T>(1) - this->x, static_cast<T>(m + 1)); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	m_fact_1mx_mp1_inverse_series() = delete;


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
	m_fact_1mx_mp1_inverse_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
m_fact_1mx_mp1_inverse_series<T, K>::m_fact_1mx_mp1_inverse_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be < 1");
	}

	TermCalculatorBase<T,K>::series_name = "m!/(1-x)^(m+1)";
	TermCalculatorBase<T, K>::x = config.x;
	m_fact_1mx_mp1_inverse_series<T, K>::m = config.addKParameter;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T m_fact_1mx_mp1_inverse_series<T, K>::calculateTerm(K n) const {
	T coeff = m;
	for(K j = 1; j <=n ; ++j){ coeff *= (m + static_cast<T>(j)) / static_cast<T>(j); }

	return static_cast<T>(fact<K>(m)) * pow(this->x, static_cast<T>(n)) * coeff;
}
