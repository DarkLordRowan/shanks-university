#pragma once

#include <stdexcept>
#include <string>

#include "../custom_concepts.hpp"
#include "../utils.hpp"

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
	series_base(std::string name = "unknown") : x_(0), sum(0), series_name(name) {}

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
	virtual SeriesResult<T> generate_series(
		const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
	) = 0;


protected:

	void init_vecs_with_prec(std::vector<T>& vecSn, std::vector<T>& vecAn, size_t vecSize, const T& x){
		size_t precision = 0;
    	if constexpr ( std::is_same<T, float_precision> :: value ){
			precision = x.precision();
			vecAn = std::vector<float_precision>(vecSize, float_precision(0.0, precision));
			vecSn = std::vector<float_precision>(vecSize, float_precision(0.0, precision));
		} else if constexpr (
			std::is_same<T, complex_precision<float_precision>>::value ||
			std::is_same<T, std::complex<float_precision>>::value
		){
			precision = std::max(x.real().precision(), x.imag().precision());
			vecAn = std::vector<T>(vecSize, T(float_precision(0.0, precision),float_precision(0.0, precision)));
			vecSn = std::vector<T>(vecSize, T(float_precision(0.0, precision),float_precision(0.0, precision)));
		} else {
			vecSn = std::vector<T>(vecSize, static_cast<T>(0));
			vecAn = std::vector<T>(vecSize, static_cast<T>(0));
		}
	}

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
};