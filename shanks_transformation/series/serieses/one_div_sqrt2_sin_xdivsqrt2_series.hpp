#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <AcceptedLike T, std::unsigned_integral K>
class one_div_sqrt2_sin_xdivsqrt2_series final : public TermCalculatorBase<T, K>
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
	T calculate_sum() const  { return sin(this->x / sqrt(static_cast<T>(2))) / sqrt(static_cast<T>(2)); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	one_div_sqrt2_sin_xdivsqrt2_series() = delete;


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
	one_div_sqrt2_sin_xdivsqrt2_series(const SeriesConfig<T,K>& config);
};

template <AcceptedLike T, std::unsigned_integral K>
one_div_sqrt2_sin_xdivsqrt2_series<T, K>::one_div_sqrt2_sin_xdivsqrt2_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x is not finite");
	}

	TermCalculatorBase<T,K>::series_name = "(1/√2)*sin(x/√2)";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <AcceptedLike T, std::unsigned_integral K>
constexpr T one_div_sqrt2_sin_xdivsqrt2_series<T, K>::calculateTerm(K n) const {

	if constexpr (std::is_floating_point<T>::value || std::is_same<T, float_precision>::value){

		//float_precision could be converted to double
		#ifdef _WIN32
    	    return static_cast<T>(pow(-1, n / 2) * _jn(static_cast<int>(2 * n + 1), this->x)); // (96.1) [Rows.pdf]
    	#else
    	    return static_cast<T>(pow(-1, n / 2) * jn(static_cast<T>(2 * n + 1), this->x));
    	#endif

	} else if constexpr ( std::is_same<T, complex_precision<float_precision>>::value){

		T term = pow(this->x * static_cast<T>(0.5), static_cast<T>(2*n + 1))/static_cast<T>(fact<K>(2*n + 1));
		T result = term;
		K k=1;

		while (abs(term) > float_precision(1e-16)){
			term *= static_cast<T>(-0.25) * this->x * this->x / static_cast<T>(k * (2*n + 1 + k));
			result += term;
			++k;
		}

		return static_cast<T>(pow(-1, n / 2)) * result;
	}
}
