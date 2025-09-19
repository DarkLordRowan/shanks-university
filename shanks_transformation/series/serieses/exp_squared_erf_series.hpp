#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class exp_squared_erf_series final : public TermCalculatorBase<T, K>
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

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value){
			return exp(this->x * this->x) * erf(this->x);
		}

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value ){
			return static_cast<T>(0);
		}
	}

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	exp_squared_erf_series() = delete;


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
	exp_squared_erf_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
exp_squared_erf_series<T, K>::exp_squared_erf_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x is not finite");
	}

	TermCalculatorBase<T,K>::series_name = "exp(x²)*erf(x)";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T exp_squared_erf_series<T, K>::calculateTerm(K n) const {
	return pow(this->x, static_cast<T>(fma(2, n, 1))) / static_cast<T>(tgamma(n + 1.5)); // (10.3) [Rows.pdf]
}