#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class ln_cosx_series final : public TermCalculatorBase<T, K>
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

		if constexpr ( std::is_floating_point<T>::value)
			return abs(config.x)  >= static_cast<T>(PI * 0.5) || !isfinite(config.x); 

        if constexpr (std::is_same<T, float_precision>::value)
            return abs(config.x)  >= static_cast<T>(0.5) * arbPI || !isfinite(config.x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(config.x) > static_cast<float_precision>(0.5) * arbPI || !isfinite(config.x); ; 
		
		return false;
	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return log(cos(this->x)); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	ln_cosx_series() = delete;


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
	ln_cosx_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
ln_cosx_series<T, K>::ln_cosx_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be < π/2");
	}

	TermCalculatorBase<T,K>::series_name = "ln(cos(x))";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T ln_cosx_series<T, K>::calculateTerm(K n) const {
	const T a = static_cast<T>(fma(2, n, 2));
    return static_cast<T>(-1) * pow(sin(this->x), a) / a; // (50.2) [Rows.pdf]
}