#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class eighth_pi_m_one_third_series final : public TermCalculatorBase<T, K>
{
protected:

    /**
     * @brief 
     * 
     * @param x 
     * @return true 
     * @return false 
     */
    inline bool domain_checker(const SeriesConfig<T,K>& config) const { return !isfinite(config.x); }

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  {

        T res = static_cast<T>(-1)/static_cast<T>(3);

		if constexpr ( std::is_floating_point<T>::value){
			res += static_cast<T>(0.125 * PI);
		}

		if constexpr ( std::is_same<T, float_precision>::value || std::is_same<T, complex_precision<float_precision>>::value){
			res +=static_cast<T>(0.125) * static_cast<T>(arbPI);
        }

        return this->x * res;
	}

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	eighth_pi_m_one_third_series() = delete;


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
	eighth_pi_m_one_third_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
eighth_pi_m_one_third_series<T, K>::eighth_pi_m_one_third_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x is not finite");
	}

	TermCalculatorBase<T,K>::series_name = "(π/8 - 1/3)*x";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T eighth_pi_m_one_third_series<T, K>::calculateTerm(K n) const {
	const K a = static_cast<K>(fma(2, n, 1));
    return minus_one_raised_to_power_n<T,K>(n) * this->x / static_cast<T>(a * (a + static_cast<K>(2)) * (a + static_cast<K>(4))); // (28.2) [Rows.pdf]
}