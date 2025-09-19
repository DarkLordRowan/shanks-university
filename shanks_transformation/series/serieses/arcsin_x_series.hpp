#pragma once
#include "../term_calculator.hpp"

/**
* @brief Taylor series of function arcsin(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class arcsin_x_series final : public TermCalculatorBase<T, K>
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
	T calculate_sum() const  { return asin(this->x); }

public:

    /**
     * @brief Construct a new arcsin x series object
     */
    arcsin_x_series() = delete;

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] constexpr virtual T calculateTerm(K n) const override;

    /**
	 * @brief 
	 * 
	 * @param config 
	 */
	arcsin_x_series(const SeriesConfig<T,K>& config);
    
};

template <Accepted T, std::unsigned_integral K>
arcsin_x_series<T, K>::arcsin_x_series(const SeriesConfig<T,K>& config){

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be ≤ 1");
	}

	TermCalculatorBase<T,K>::series_name = "arcsin(x)";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T arcsin_x_series<T, K>::calculateTerm(K n) const {
    const T factorial = static_cast<T>(fact<K>(n));
    const T coeff = static_cast<T>(fact<K>(2*n)) / factorial / factorial / static_cast<T>(pow(2, 2*n));
    return coeff * pow(this->x, static_cast<T>(fma(2,n,1))) / static_cast<T>(fma(2,n,1)); // (69.1) [Rows.pdf]
}
