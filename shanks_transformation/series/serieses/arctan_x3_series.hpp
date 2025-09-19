#pragma once
#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of function arctan(x^3)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class arctan_x3_series final : public TermCalculatorBase<T, K>
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
	T calculate_sum() const  { return atan(pow(this->x, static_cast<T>(3))); }

public:
    /**
     * @brief Construct a new arctan x3 series object
     * 
     */

    arctan_x3_series() = delete;


    /**
    * @brief Computes the nth term of the arctanx3 series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the arctanx3 series
    */
    [[nodiscard]] constexpr virtual T calculateTerm(K n) const override;
    
    /**
	 * @brief 
	 * 
	 * @param config 
	 */
	arctan_x3_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
arctan_x3_series<T, K>::arctan_x3_series(const SeriesConfig<T,K>& config){

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be ≤ 1");
	}

	TermCalculatorBase<T,K>::series_name = "arctan(x³)";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}


template <Accepted T, std::unsigned_integral K>
constexpr T arctan_x3_series<T, K>::calculateTerm(K n) const
{
    const T a = static_cast<T>(fma(static_cast <K>(2), n, static_cast <K>(1)));
    return minus_one_raised_to_power_n<T,K>(n) * pow(this->x, static_cast<T>(3) * a) / a; // (83.2) [Rows.pdf],
}
