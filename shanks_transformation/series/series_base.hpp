#pragma once

#include <vector>
#include <concepts>
#include <string>
#include <memory>

#include "term_calculator.hpp"
#include "noise_generators/uniform_noise_gen.hpp"


 /**
 * @brief Abstract class for series
 * @authors Bolshakov M.P.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
template <AcceptedLike T, std::unsigned_integral K>
class series_base
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/

	series_base() = delete;

	explicit series_base(
		std::unique_ptr<TermCalculatorBase<T, K>> termCalculator, 
		noise_gen_id gen_id = noise_gen_id::none_id, 
		size_t size = 20
	) : termCalculator(std::move(termCalculator)), size(0) {

		if(noise_gen_id::uniform_id == noiseGenId){
			//пока так я не знаю что делать
			noiseGenerator.reset(new UniformNoiseGenerator<T>(static_cast<T>(-0.005), static_cast<T>(0.005)));
		}

		resize_vecs(size);
	}


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

	[[nodiscard]] constexpr const std::string get_name() const { return termCalculator->get_name(); };


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
	size_t size = 0;

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

	noise_gen_id noiseGenId = noise_gen_id::none_id;

	/**
	 * @brief 
	 * 
	*/
	std::unique_ptr<BaseNoiseGenerator<T>> noiseGenerator;
	//UniformNoiseGenerator<T> noiseGenerator = UniformNoiseGenerator<T>

};

template<AcceptedLike T, std::unsigned_integral K>
constexpr void series_base<T, K>::resize_vecs(const K n){


	size_t old_size = size;
	size_t new_size = static_cast<size_t>(3 * n / 2 + 1);

	size = new_size;

	a_nVec.resize(new_size, static_cast<T>(0));
	S_nVec.resize(new_size, static_cast<T>(0));

	if(noise_gen_id::none_id != noiseGenId){ randomVec.resize(new_size, static_cast<T>(0)); }

	for(size_t i = old_size; i < new_size; ++i ){

		try{
			a_nVec[i] = termCalculator->calculateTerm(i);
		} catch (...){} //заглушка

		if(noise_gen_id::none_id != noiseGenId){
			randomVec[i] = noiseGenerator->operator()(i);
			a_nVec[i] += randomVec[i];
		}

		S_nVec[i] = (i ? S_nVec[i - 1] : static_cast<T>(0)) + a_nVec[i];
	}

}


template <AcceptedLike T, std::unsigned_integral K>
constexpr T series_base<T, K>::Sn(K n) {

	if(n >= size){ resize_vecs(n); }

	return S_nVec[n];

}

template <AcceptedLike T, std::unsigned_integral K>
constexpr T series_base<T, K>::an(K n) {

	if(n >= size){ resize_vecs(n); }

	return a_nVec[n];

}

template <AcceptedLike T, std::unsigned_integral K>
constexpr const T series_base<T, K>::get_x() const
{
	return termCalculator->get_x();
}

template <AcceptedLike T, std::unsigned_integral K>
constexpr const T series_base<T, K>::get_sum() const
{
	return termCalculator->get_sum();
}