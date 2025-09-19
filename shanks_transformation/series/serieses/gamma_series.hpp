#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class gamma_series final : public TermCalculatorBase<T, K>
{
protected:

    T t;

    /**
     * @brief 
     * 
     * @param x 
     * @return true 
     * @return false 
     */
    inline bool domain_checker(const SeriesConfig<T,K>& config) const { 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return config.addTParameter < static_cast<T>(0) || !isfinite(config.x) || !isfinite(config.addTParameter); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return config.addTParameter.real() < static_cast<float_precision>(0) || !isfinite(config.x) || !isfinite(config.addTParameter); 
		
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
	gamma_series() = delete;


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
	gamma_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
gamma_series<T, K>::gamma_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("t must be > 0 and parameters must be finite");
	}

	TermCalculatorBase<T,K>::series_name = "Γ(t,x)";
	TermCalculatorBase<T, K>::x = config.x;
    gamma_series<T,K>::t = config.addTParameter;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T gamma_series<T, K>::calculateTerm(K n) const {
	const T n_1 = static_cast<T>(n+1);
    return n_1 * pow(this->t, static_cast<T>(n)) * tgamma(n_1); // (102.1) [Rows.pdf]
}