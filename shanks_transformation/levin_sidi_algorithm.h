/**
* @file levin_sidi_merg.h
* @brief This files contains the definition of Levin-Sidi S-transformation with u,t,v remainders
*/
#pragma once
#define DEF_UNDEFINED_SUM 0
#define BETA 1

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library


template<typename T, typename K>
class transform_base {
public:

	/**
	* @brief Default constructor for abstract remainder functor for S-tranformation
	* @authors Naumov A.
	*/

	virtual T operator()( const int& n, const int& j, const series_base<T, K>* series, T scale = T(1)) const = 0;

};

template<typename T, typename K>
class u_transform : public transform_base<T,K> {
public:
	/**
	* @brief Default constructor for u type remainder functor for S-tranformation, for more information see p. 43 7.3-4 in [https://arxiv.org/pdf/math/0306302.pdf]
	* @authors Naumov A.
	*/
	u_transform() {}

	T operator()(const int& n, const int& j, const series_base<T, K>* series, T scale = T(1)) const {
		return 1 / (scale * series->operator()(n + j + 1));
	}
};

template<typename T, typename K>
class t_transform : public transform_base<T, K> {
public:
	/**
	* @brief Default constructor for t type remainder functor for S-tranformation, for more information see p. 60 8.4-4 in [https://arxiv.org/pdf/math/0306302.pdf]
	* @authors Naumov A.
	*/
	t_transform() {}

	T operator()(const int& n, const int& j, const series_base<T, K>* series, T scale = T(1)) const {
		return 1 / series->operator()(n + j);
	}
};

template<typename T, typename K>
class v_transform : public transform_base<T, K> {
public:
	/**
	* @brief Default constructor for v type remainder functor for S-tranformation, for more information see p. 60 8.4-5 in [https://arxiv.org/pdf/math/0306302.pdf]
	* @authors Naumov A.
	*/
	v_transform() {}

	T operator()(const int& n, const int& j, const series_base<T, K>* series, T scale = T(1)) const {
		return (series->operator()(n + j + 1) - series->operator()(n + j)) / (series->operator()(n + j + 1) * series->operator()(n + j));
	}
};


template<typename T, typename K, typename series_templ>
class levi_sidi_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	const transform_base<T, K>* remainder_func;

	bool recursive;

	/**
	* @brief Default function to calculate S-tranformation directly by formula. Implemented u,t and v transformations. For more information see p. 57 8.2-7 [https://arxiv.org/pdf/math/0306302.pdf]
	* Levin-Sidi or Factorial analog of Levin Transformation is effective for series that belong to b(1)/LIN/FAC and inferior on b(1)/LOG for more information see p. 369 and p.285 [http://servidor.demec.ufpr.br/CFD/bibliografia/MER/Sidi_2003.pdf]
	* @authors Naumov A.
	* @param k The number of terms in the partial sum.
	* @param n the order of transformation
	* @return The partial sum after the transformation.
	*/

	virtual T calculate(const K& k, const int& n) const {

		if (n < 0)
			throw std::domain_error("negative integer in input");
		if (BETA <= 0)
			throw std::domain_error("beta cannot be initiared by a negative number or a zero");


		T numerator = T(0), denominator = T(0);
		T w_n, rest;
		T up, down;

		for (int j = 0; j <= k; ++j) {

			rest = this->series->minus_one_raised_to_power_n(j) * this->series->binomial_coefficient(k, j);

			up = down = T(1);
			for (int m = 0; m < k - 1; ++m) {
				up *= (BETA + n + j + m);
				down *= (BETA + n + k + m);
			}

			rest = rest * (up / down);

			w_n = remainder_func->operator()(n, j, this->series, BETA + n);

			numerator += rest * this->series->S_n(n + j) * w_n;
			denominator += rest * w_n;

		}

		numerator /= denominator;

		if (!std::isfinite(numerator))
			throw std::overflow_error("division by zero");

		return numerator;
	}

	/**
	* @brief Default function to calculate S-tranformation using reccurence formula. Implemented u,t and v transformations. For more information see p. 57 8.3-5 [https://arxiv.org/pdf/math/0306302.pdf]
	* Levin-Sidi or Factorial analog of Levin Transformation is effective for series that belong to b(1)/LIN/FAC and inferior on b(1)/LOG for more information see p. 369 and p.285 [http://servidor.demec.ufpr.br/CFD/bibliografia/MER/Sidi_2003.pdf]
	* @authors Naumov A.
	* @param k The number of terms in the partial sum.
	* @param n the order of transformation
	* @param remainder_func functor, whose returning w_n for t,u or v transformation
	* @return The partial sum after the transformation.
	*/

	T calculate_rec(const K& k, const int& n) const {

		if (n < 0)
			throw std::domain_error("negative integer in input");
		if (BETA <= 0)
			throw std::domain_error("beta cannot be initiared by a negative number or a zero");

		std::vector<T>* N = new std::vector<T>(k + 1, 0);
		std::vector<T>* D = new std::vector<T>(k + 1, 0);

		for (int i = 0; i < k + 1; i++) {
			(*D)[i] = remainder_func->operator()(0, n + i, this->series);
			(*N)[i] = this->series->S_n(n + i) * (*D)[i];
		}

		for (int i = 1; i <= k; ++i) {
			for (int j = 0; j <= k - i; ++j) {

				T scale1 = ((BETA + n + j + i) * (BETA + n + j + i - 1));
				T scale2 = ((BETA + n + j + 2 * i) * (BETA + n + j + 2 * i - 1));

				(*D)[j] = ((*D)[j + 1] * scale2 - scale1 * (*D)[j]) / scale2;
				(*N)[j] = ((*N)[j + 1] * scale2 - scale1 * (*N)[j]) / scale2;
			}
		}

		T numerator = (*N)[0] / (*D)[0];

		if (!std::isfinite(numerator))
			throw std::overflow_error("division by zero");

		return numerator;
	}

public:

	/**
	* @brief generalised Levi-Sidi class template for derivations
	* @authors Naumov A.
	* @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
	* @param 
	*/

	levi_sidi_algorithm(const series_templ& series, const transform_base<T, K>* func, bool recursive = false) : series_acceleration<T, K, series_templ>(series), remainder_func(func), recursive(recursive) {}

	~levi_sidi_algorithm() { delete remainder_func; }

	/**
	* @brief Abstract method for the inherited classes below
	* Computes the partial sum after the transformation using the Levin Algorithm.
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @return The partial sum after the transformation.
	*/

	T operator()(const K k, const int n) const {
		if (recursive) return calculate_rec(k, n);
		return calculate(k, n);
	}

};
