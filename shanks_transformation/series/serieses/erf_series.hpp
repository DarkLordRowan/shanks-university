#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class erf_series final : public TermCalculatorBase<T, K>
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

		if(std::is_same<T, complex_precision<float_precision>>::value){ return static_cast<T>(0);}

		if constexpr ( std::is_floating_point<T>::value){
			return static_cast<T>(0.5) * sqrt(static_cast<T>(PI))* erf(this->x);
		}

		if constexpr ( std::is_same<T, float_precision>::value){
			return static_cast<T>(0.5) * sqrt(arbPI) * erf(this->x);
        }
		return static_cast<T>(0);
	}

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	erf_series() = delete;


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
	erf_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
erf_series<T, K>::erf_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x is not finite");
	}

	TermCalculatorBase<T,K>::series_name = "√π/2 * erf(x)";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T erf_series<T, K>::calculateTerm(K n) const {
	const T two_n_1 = static_cast<T>(fma(2,n,1));

	return minus_one_raised_to_power_n<T,K>(n) * static_cast<T>(2) * pow(this->x, two_n_1) / (sqrt(static_cast<T>(PI)) * static_cast<T>(fact<K>(n)) * two_n_1);
}
