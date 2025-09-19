#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class xmb_Jb_two_series final : public TermCalculatorBase<T, K>
{
protected:

    K mu;

    /**
     * @brief 
     * 
     * @param x 
     * @return true 
     * @return false 
     */
    inline bool domain_checker(const SeriesConfig<T,K>& config) const { return !isfinite(config.x) || config.x == static_cast<T>(0); }

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  {

		if constexpr ( std::is_floating_point<T>::value ){
			return static_cast<T>(1) / pow(this->x, static_cast<T>(mu)) * std::cyl_bessel_j(static_cast<T>(mu), static_cast<T>(2) * this->x);
		}

		if constexpr ( std::is_same<T, float_precision>::value || std::is_same<T, complex_precision<float_precision>>::value){
			return static_cast<T>(0);
		}

	}

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	xmb_Jb_two_series() = delete;


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
	xmb_Jb_two_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
xmb_Jb_two_series<T, K>::xmb_Jb_two_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x must be finite and non-zero");
	}

	TermCalculatorBase<T,K>::series_name = "x⁻ᵇJᵦ(2x)";
	TermCalculatorBase<T, K>::x = config.x;
    xmb_Jb_two_series<T,K>::mu = config.addKParameter;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T xmb_Jb_two_series<T, K>::calculateTerm(K n) const {
	return minus_one_raised_to_power_n<T,K>(n) * pow(this->x,static_cast<T>( 2 * n))
        / (static_cast<T>(fact(n)) * static_cast<T>(tgamma(n + this->mu + 1))); // (11.3) [Rows.pdf]
}