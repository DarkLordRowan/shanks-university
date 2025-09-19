#pragma once
#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of function arcsin(x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class arcsin_x2_series final : public TermCalculatorBase<T, K>
{
protected:

    /**
     * @brief 
     * 
     * @param x 
     * @return true 
     * @return false 
     */
    inline bool domain_checker(const SeriesConfig<T,K>& config) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return config.x > static_cast<T>(1.0) || !isfinite(config.x); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return config.x.real() > static_cast<float_precision>(1.0) || !isfinite(config.x); 
		
		return false;

	}

    /**
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
    T calculate_sum() const  { return asin(this->x * this->x); }
    
public:

    /**
    * @brief Construct a new arcsin x2 series object
    * 
    */
    arcsin_x2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */

    /**
    * @brief Computes the nth term of the arcsinx2 series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the arcsinx2 series
    */
    [[nodiscard]] constexpr virtual T calculateTerm(K n) const override;

    /**
	 * @brief 
	 * 
	 * @param config 
	 */
	arcsin_x2_series(const SeriesConfig<T,K>& config);
    
};

template <Accepted T, std::unsigned_integral K>
arcsin_x2_series<T, K>::arcsin_x2_series(const SeriesConfig<T,K>& config){

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be ≤ 1");
	}

	TermCalculatorBase<T,K>::series_name = "arcsin(x²)";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T arcsin_x2_series<T, K>::calculateTerm(K n) const {
    const K a = static_cast<K>(fma(static_cast <K>(2), n, static_cast <K>(1)));
    return static_cast<T>(fact(a - static_cast <K>(1))) * pow(this->x, static_cast<T>(static_cast <K>(2) * a)) / static_cast<T>(pow(static_cast <K>(4), n) * pow(fact<K>(n), static_cast <K>(2)) * a); // (84.1) [Rows.pdf]
}
