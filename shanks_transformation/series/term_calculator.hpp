#pragma once

#include <math.h>
#include <cmath>
#include <concepts>
#include <string>
#include <numbers>

#include "../custom_concepts.hpp"

using std::isfinite;
using std::to_string;

static const float_precision arbPI = float_precision(_float_table(_PI, float_precision_ctrl.precision()));
static const double PI = std::numbers::pi;

template<Accepted T, std::unsigned_integral K>
struct SeriesConfig{
	T x;
	T addTParameter;
	K addKParameter;

	SeriesConfig(
		const T& x = static_cast<T>(0), 
		const T& addTParameter = static_cast<T>(0), 
		const K addKParameter = static_cast<K>(0)
	) : x(x), addTParameter(addTParameter), addKParameter(addKParameter) {}
};


 /**
 * @brief Abstract class for series
 * @authors Bolshakov M.P.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
template <Accepted T, std::unsigned_integral K>
class TermCalculatorBase
{

protected:

	/**
	* @brief function series argument
	* It's set to 0 by default
	* @authors Bolshakov M.P.
	*/
	T x;

	/**
	* @brief sum of the series
	* It's set to 0 by default
	* @authors Bolshakov M.P.
	*/
	T sum;

	/**
    * @brief Name of the series for unified error messages
    * Used to generate consistent error output format
    * @authors Maximov A.K.
    */
	std::string series_name;

	/**
	 * @brief 
	 * 
	 * @param config 
	 * @return true 
	 * @return false 
	 */
	virtual inline bool domain_checker(const SeriesConfig<T,K>& config) const = 0;

	/**
	 * @brief 
	 * 
	 * @return constexpr T 
	 */
	virtual T calculate_sum() const = 0;

public:

	/**
	* @brief x getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr const T get_x() const;

	/**
	* @brief sum getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr const T get_sum() const;

	/**
	* @brief sum getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr const std::string get_name() const;

	void throw_domain_error(const std::string& condition) const {
		throw std::domain_error(series_name + " series diverges at x = " + to_string(x) + " (" + condition + ")");
	}

	TermCalculatorBase() {};
	TermCalculatorBase(const SeriesConfig<T,K>& config) {};

	/**
	 * @brief 
	 * 
	 * @param n 
	 * @return T 
	 */
    inline virtual T calculateTerm(K n) const = 0;

};

template <Accepted T, std::unsigned_integral K>
constexpr const T TermCalculatorBase<T, K>::get_x() const { return x; }

template <Accepted T, std::unsigned_integral K>
constexpr const T TermCalculatorBase<T, K>::get_sum() const { return sum; }

template <Accepted T, std::unsigned_integral K>
constexpr const std::string TermCalculatorBase<T, K>::get_name() const { return series_name; }


/**
* @brief factorial k!
* @authors Bolshakov M.P.
* @return k!
*/
template<std::unsigned_integral K>
[[nodiscard]] constexpr const K fact(K n){
	K f = 1;
	for (K i = 2; i <= n; ++i)
		f *= i;
	return f;
}

/**
* @brief double factorial k!!
* @authors Bolshakov M.P.
* @return k!!
*/
template<std::unsigned_integral K>
[[nodiscard]] constexpr const K double_fact(K n){
	if (n == 0 || n == 1)
		return 1;
	return n * double_fact(n - 2);
}

/**
* @brief binomial coefficient C^n_k
* @authors Bolshakov M.P.
* @return combinations(n,k)
*/
template<Accepted T, std::unsigned_integral K>
[[nodiscard]] constexpr const T binomial_coefficient(const T n, const K k){
	T b_c = static_cast<T>(1);
	for (K i = 0; i < k; ++i)
		b_c *= (n - static_cast<T>(i)) / static_cast<T>(i + 1);
	return b_c;
}

/**
* @brief evaluates (-1)^n
* @authors Bolshakov M.P.
* @return (-1)^n
*/
template<Accepted T, std::unsigned_integral K>
[[nodiscard]] constexpr const T minus_one_raised_to_power_n(K n){
	return static_cast<T>(n & 1 ? -1.0 : 1.0);
}

/**
* @brief evaluates Euler function by n
* @authors Trudolyubov N.A.
* @return phi(n)
*/
template<Accepted T, std::unsigned_integral K>
[[nodiscard]] constexpr const T phi(K n){
	K result = n;
	for (K i = 2; i * i <= n; ++i)
		if (n % i == 0) {
			while (n % i == 0)
				n /= i;
			result -= result / i;
		}

	result -= n > 1 ? result / n : 0;
	return static_cast<T>(result);
}