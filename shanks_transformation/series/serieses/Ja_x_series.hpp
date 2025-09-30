#pragma once

#include "../term_calculator.hpp"
#include <cmath>
#include <type_traits>

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, std::unsigned_integral K>
class Ja_x_series final : public TermCalculatorBase<T, K>
{
protected:

    T a;

    /**
     * @brief 
     * 
     * @param x 
     * @return true 
     * @return false 
     */
    inline bool domain_checker(const SeriesConfig<T,K>& config) const override { return !isfinite(config.x) || !isfinite(config.addTParameter); }

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
	Ja_x_series() = delete;


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
	Ja_x_series(const SeriesConfig<T,K>& config);
};

template <AcceptedLike T, std::unsigned_integral K>
Ja_x_series<T, K>::Ja_x_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x and a must be finite");
	}

	TermCalculatorBase<T,K>::series_name = "Jₐ(x)";
	TermCalculatorBase<T, K>::x = config.x;
    Ja_x_series<T,K>::a = config.addTParameter;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <AcceptedLike T, std::unsigned_integral K>
constexpr T Ja_x_series<T, K>::calculateTerm(K n) const {
	if constexpr(std::is_same<T, float_precision>::value || std::is_floating_point<T>::value){
		return minus_one_raised_to_power_n<T,K>(n) * pow(this->x * static_cast <T>(0.5), static_cast<T>(2 * n) + this->a) / 
		(static_cast<T>(fact<K>(n)) * tgamma(this->a + static_cast<T>(n + 1))); // (95.1) [Rows.pdf]
	} else if constexpr(std::is_same<T, complex_precision<float_precision>>::value) {
		//using approximation for gamma function from https://en.wikipedia.org/wiki/Stirling%27s_approximation
		const T z = this->a + static_cast<T>(n + 1);
		T result = minus_one_raised_to_power_n<T,K>(n) * pow(this->x * static_cast <T>(0.5), static_cast<T>(2 * n) + this->a);
		result /= static_cast<T>(fact<K>(n));
		result /= sqrt(static_cast<T>(2) * static_cast<T>(arbPI) / z) * pow( 
			static_cast<T>(1/std::numbers::e) * (z + static_cast<T>(1)/(static_cast<T>(12)*z - static_cast<T>(0.1)/z)),
			z
		); //using Gego Nemes approximation
		return result;
	}
}
