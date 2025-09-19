#pragma once
#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of function arth(x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class arctanh_x2_series final : public TermCalculatorBase<T, K>
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
	T calculate_sum() const  { return atanh(this->x * this->x); }

public:

    arctanh_x2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @param x The argument for function series
    */
    arctanh_x2_series(
		const T& x = static_cast<T>(0), 
		size_t size = 50,
		bool noise = false
	);

    /**
    * @brief Computes the nth term of the arctanhx2 series
    * @authors Pavlova A.R.
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
	arctanh_x2_series(const SeriesConfig<T,K>& config);
    
};

template <Accepted T, std::unsigned_integral K>
arctanh_x2_series<T, K>::arctanh_x2_series(const SeriesConfig<T,K>& config){

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be < 1");
	}

	TermCalculatorBase<T,K>::series_name = "arctanh(x²)";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T arctanh_x2_series<T, K>::calculateTerm(K n) const {
    const T a = static_cast<T>(fma(2, n, 1));
    return pow(this->x, static_cast<T>(2) * a) / a; // (90.2) [Rows.pdf]
}
