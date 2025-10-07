#pragma once

#include <stdexcept>
#include <string>

#include "../custom_concepts.hpp"

template<UnsignedIntLike K>
constexpr const K fact(const K n) {
	K fact = static_cast<K>(1);
	for(K j = static_cast<K>(2); j <= n; ++j){
		fact *= j;
	}
	return fact;
}

template<UnsignedIntLike K>
constexpr const K double_fact(const K n) {

	K double_fact = static_cast<K>(1);

	for (K j = n & static_cast<K>(1) + static_cast<K>(2); j <= n; j+=2){
		double_fact *= j;
	}

	return double_fact;
}

template<UnsignedIntLike K>
constexpr const K binomial_coefficient(const K n, const K k) {


	if (k == 0 || k == n) return 1;

	return binomial_coefficient<K>(n - 1, k - 1) + binomial_coefficient<K>(n - 1, k);

}

 /**
 * @brief Abstract class for series
 * @authors Bolshakov M.P.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
template <AcceptedLike T, UnsignedIntLike K>
class series_base
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/
	series_base();

	/**
	* @brief x getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr const T get_x() const { return x_; }

	/**
	* @brief sum getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr const T get_sum() const { return sum; }

	/**
	* @brief sum getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr const std::string get_name() const { return series_name; }

	/**
	 * @brief 
	 * 
	 * @param vecSize 
	 * @return std::vector<T> 
	 */
	virtual SeriesResult<T> generateSeries(
		const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
	) = 0;


protected:

	/**
    * @brief Throws domain error with unified message format
    * @authors Maximov A.K.
    * @param condition Description of the divergence condition
    * @throws std::domain_error with formatted message containing series name, x value and condition
    */
	void throw_domain_error(const std::string& condition) const {

		using std::to_string;

		throw std::domain_error(series_name + " series diverges at x = " + to_string(x_) + " (" + condition + ")");
	}

	/**
	* @brief function series argument
	* It's set to 0 by default
	* @authors Bolshakov M.P.
	*/
	T x_;

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

	size_t precision;
};

template <AcceptedLike T, UnsignedIntLike K>
series_base<T, K>::series_base() : x_(0), sum(0), series_name("unknown") {}