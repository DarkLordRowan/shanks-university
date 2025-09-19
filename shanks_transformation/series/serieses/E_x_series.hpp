#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class E_x_series final : public TermCalculatorBase<T, K>
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
			return abs(config.x) >= static_cast<T>(1.0) || !isfinite(config.x); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(config.x.real()) >= static_cast<float_precision>(1.0) || !isfinite(config.x); 
		
		return false;

	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return static_cast<T>(0); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	E_x_series() = delete;


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
	E_x_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
E_x_series<T, K>::E_x_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be < 1");
	}

	TermCalculatorBase<T,K>::series_name = "E(x)";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T E_x_series<T, K>::calculateTerm(K n) const {
	if(n == 0)
        return static_cast<T>(PI * 0.5);

    const T coeff = static_cast<T>(double_fact<K>(2*n-1)) / static_cast<T>(double_fact<K>(2*n));
    return static_cast<T>(-PI* 0.5) * coeff * coeff * pow(this->x, static_cast<T>(2*n)) / static_cast<T>(2*n-1); // (72.1) [Rows.pdf]
}