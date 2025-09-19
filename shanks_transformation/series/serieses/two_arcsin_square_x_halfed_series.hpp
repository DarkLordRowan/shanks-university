#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class two_arcsin_square_x_halfed_series final : public TermCalculatorBase<T, K>
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
			return abs(config.x) > static_cast<T>(2) || !isfinite(config.x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value)
			return abs(config.x) > static_cast<float_precision>(2) || !isfinite(config.x);
		
		return false;
	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return static_cast<T>(2) * asin(this->x * static_cast<T>(0.5)) * asin(this->x * static_cast<T>(0.5)); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	two_arcsin_square_x_halfed_series() = delete;


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
	two_arcsin_square_x_halfed_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
two_arcsin_square_x_halfed_series<T, K>::two_arcsin_square_x_halfed_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be ≤ 2");
	}

	TermCalculatorBase<T,K>::series_name = "2*arcsin(x/2)^2";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T two_arcsin_square_x_halfed_series<T, K>::calculateTerm(K n) const {
	const K a = static_cast<K>(fma(2, n, 1));
	const T fact_N = static_cast<T>(fact<K>(n));
	return pow(this->x, static_cast<T>(a - 1)) * fact_N * fact_N / (static_cast<T>(a) * static_cast<T>(fact<K>(a)));
}