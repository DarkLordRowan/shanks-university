/**
* @file drummon_D_algorithm.h
* @brief Contains implementation of Drummond's D-transformation
* @authors Naumov A.
*/

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library

/**
 * @brief D_transformation class template.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 * @param remainder_func - remainder type
 * @param recursive To calculate reccursively
*/
template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class drummonds_algorithm : public series_acceleration<T, K, series_templ>
{
protected:
	const transform_base<T, K>* const remainder_func;
	const bool recursive;

	/**
	* @brief Function to calculate D-tranformation directly by formula. For more information see p. 70 9.5-4 [https://arxiv.org/pdf/math/0306302.pdf]
	* @param n The number of terms in the partial sum.
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/

	virtual T calculate(const K n, const K order) const {
		T numerator = T(0), denominator = T(0);
		T w_n, rest, a1;

		for (K j = 0; j <= n; ++j) {
			rest = this->series->minus_one_raised_to_power_n(j) * this->series->binomial_coefficient(static_cast<T>(n), j);
			w_n = remainder_func->operator()(order, j, this->series, 1);
			a1 = rest * w_n;

			numerator += a1 * this->series->S_n(order + j);
			denominator += a1;
		}

		numerator /= denominator;

		if (!std::isfinite(numerator))
			throw std::overflow_error("division by zero");

		return numerator;
	}

	/**
	* @brief Function to calculate D-tranformation using reccurence formula. For more information see p. 70 9.5-5 [https://arxiv.org/pdf/math/0306302.pdf]
	* @param k The number of terms in the partial sum.
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/

	T calculate_rec(const K n, const K order) const {
		//TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения
		const K n1 = n + 1;
		K orderi, j1;

		std::vector<T> N (n1, 0);
		std::vector<T> D (n1, 0);

		for (K i = 0; i < n1; ++i) {
			orderi = order + i;
			D[i] = remainder_func->operator()(0, orderi, this->series);
			N[i] = this->series->S_n(orderi) * D[i];
		}

		for (K i = 1; i <= n; ++i)
			for (K j = 0; j <= n - i; ++j) {
				j1 = j + 1;
				D[j] = D[j1] - D[j];
				N[j] = N[j1] - N[j];
			}

		const T numerator = N[0] / D[0];

		if (!std::isfinite(numerator))
			throw std::overflow_error("division by zero");

		return numerator;
	}
public:

	/**
	* @brief Parameterized constructor to initialize the Drummonds Algorithm.
	* @param series The series class object to be accelerated
	* @param func Remainder function
	* @param recursive How to calculate
	*/

	drummonds_algorithm(const series_templ& series, const transform_base<T, K>* const func, const bool recursive = false) : series_acceleration<T, K, series_templ>(series), remainder_func(func), recursive(recursive) {}

	~drummonds_algorithm() { delete remainder_func; }

	/**
   * @brief D-transformation.
   * Computes the partial sum after the D-transformation
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @return The partial sum after the transformation.
   */

	T operator()(const K n, const K order) const {
		return recursive ? calculate_rec(n, order) : calculate(n, order);
	}
};