#pragma once
#include "../term_calculator.hpp"

/**
* @brief Binomial series (Maclaurin series for (1+x)^a function)
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class bin_series final : public TermCalculatorBase<T, K>
{
protected:

    T alpha;

    /**
     * @brief 
     * 
     * @param x 
     * @return true 
     * @return false 
     */
    inline bool domain_checker(const SeriesConfig<T,K>& config) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return config.x >= static_cast<T>(1.0) || !isfinite(config.x) || !isfinite(config.addTParameter); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return config.x.real() >= static_cast<float_precision>(1.0) || !isfinite(config.x) || !isfinite(config.addTParameter); 
		
		return false;

	}

    /**
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return pow(static_cast<T>(1) + this->x, alpha); }

public:

    /**
     * @brief Construct a new bin series object
     * 
     */
    bin_series() = delete;


    /**
    * @brief Computes the nth term of the Binomial series
    * @authors Bolshakov M.P.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T calculateTerm(K n) const override;

    /**
	 * @brief 
	 * 
	 * @param config 
	 */
	bin_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
bin_series<T, K>::bin_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be < 1 and parameters must be finite");
	}

	TermCalculatorBase<T,K>::series_name = "(1+x)^α";
	TermCalculatorBase<T, K>::x = config.x;
    bin_series<T,K>::alpha = config.addTParameter;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T bin_series<T, K>::calculateTerm(K n) const {
    return binomial_coefficient(alpha, n) * pow(this->x, static_cast<T>(n)); // (6.1) [Rows.pdf]
}
