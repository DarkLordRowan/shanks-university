#pragma once

#include <math.h>
#include <cmath>
#include <vector>
#include <concepts>
#include <string>
#include <chrono>
#include <random>

#include "series/term_calculator.hpp"
#include "series/series.hpp"


using std::isfinite;

#define USE_NOISE true

static const unsigned long long int seed = std::chrono::system_clock::now().time_since_epoch().count() + std::rand();
static std::mt19937_64 pseudoRNG(seed);
static std::uniform_real_distribution<double> uniformRNG(-0.001, 0.001);


 /**
 * @brief Abstract class for series
 * @authors Bolshakov M.P.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
template <Accepted T, std::unsigned_integral K>
class series_base
{
public:

	virtual ~series_base() = default;

	/**
	* @brief Parameterized constructor to initialize the series with function argument
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/

	series_base() = delete;

	explicit series_base(std::unique_ptr<TermCalculatorBase<T, K>> termCalculator) : termCalculator(std::move(termCalculator)) {}


	/**
    * @brief Throws domain error with unified message format
    * @authors Maximov A.K.
    * @param condition Description of the divergence condition
    * @throws std::domain_error with formatted message containing series name, x value and condition
    */

	/**
	* @brief Computes partial sum of the first n terms
	* @authors Bolshakov M.P.
	* @param n The amount of terms in the partial sum
	* @return Partial sum of the first n terms
	*/
	[[nodiscard]] constexpr T Sn(K n);

	/**
	* @brief Computes nth term of the series
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @return nth term of the series
	*/
	[[nodiscard]] constexpr T an(K n);

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

	[[nodiscard]] constexpr const std::string get_name() const;


protected:

	/**
	 * @brief 
	 * 
	 */
	std::unique_ptr<TermCalculatorBase<T, K>> termCalculator;

	/**
	 * @brief 
	 * 
	 * @param new_size 
	 */
	inline constexpr void resize_vecs(const K new_size);

	/**
	 * @brief 
	 * 
	 */
	size_t size;

	/**
	 * @brief 
	 * 
	 */
	bool noise = true;

	/**
	 * @brief 
	 * 
	 */
	std::vector<T> a_nVec;

	/**
	 * @brief 
	 * 
	 */
	std::vector<T> S_nVec;

	/**
	 * @brief 
	 * 
	 */
	std::vector<T> randomVec;

};

template<Accepted T, std::unsigned_integral K>
constexpr void series_base<T, K>::resize_vecs(const K n){

	size_t old_size = a_nVec.size();
	size_t new_size = static_cast<size_t>(3 * n / 2 + 1);

	a_nVec.resize(new_size, static_cast<T>(0));
	S_nVec.resize(new_size, static_cast<T>(0));

	if(noise){ randomVec.resize(new_size, static_cast<T>(0)); }

	for(size_t i = old_size; i < new_size; ++i ){

		a_nVec[i] = termCalculator->calculateTerm(i);

		if(noise){
			randomVec[i] = uniformRNG(pseudoRNG);
			a_nVec[i] += randomVec[i];
		}

		S_nVec[i] = (i ? S_nVec[i - 1] : static_cast<T>(0)) + a_nVec[i];
	}

}


template <Accepted T, std::unsigned_integral K>
constexpr T series_base<T, K>::Sn(K n) {

	if(n < size){ resize_vecs(n); }

	return S_nVec[n];

}

template <Accepted T, std::unsigned_integral K>
constexpr T series_base<T, K>::an(K n) {

	if(n < size){ resize_vecs(n); }

	return a_nVec[n];

}

template <Accepted T, std::unsigned_integral K>
constexpr const T series_base<T, K>::get_x() const
{
	return termCalculator->get_x();
}

template <Accepted T, std::unsigned_integral K>
constexpr const T series_base<T, K>::get_sum() const
{
	return termCalculator->get_sum();
}

template <Accepted T, std::unsigned_integral K>
constexpr const std::string series_base<T, K>::get_name() const
{
	return termCalculator->get_name();
}