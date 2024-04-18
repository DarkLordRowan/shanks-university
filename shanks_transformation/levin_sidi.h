/**
* @file levin_sidi_merg.h
* @brief This files contains the definition of Levin-Sidi S-transformation with u,t,v remainders
*/
#pragma once
#define DEF_UNDEFINED_SUM 0
#define BETA 1 // beta is a a nonzero positive parameter, 1 is the standart value in the Literatur on Levin transformation, for more information see p. 39 in [https://arxiv.org/pdf/math/0306302.pdf]


#include "series_acceleration.h" // Include the series header
#include <vector>
#include <iostream>

template<typename T, typename K, typename series_templ> class levi_sidi_algorithm;

template<typename T, typename K, typename series_templ> class u_levi_sidi_algorithm;

template<typename T, typename K, typename series_templ> class t_levi_sidi_algorithm;

template<typename T, typename K, typename series_templ> class v_levi_sidi_algorithm;

template<typename T, typename K, typename series_templ> class recursive_u_levi_sidi_algorithm;

template<typename T, typename K, typename series_templ> class recursive_t_levi_sidi_algorithm;

template<typename T, typename K, typename series_templ> class recursive_v_levi_sidi_algorithm;

class u_transform {
public:
	/**
	* @brief Default constructor for u type remainder calculator for S-tranformation, for more information see p. 43 7.3-4 in [https://arxiv.org/pdf/math/0306302.pdf]
	* @authors Venajalainen
	*/
	u_transform() {}

	template<typename T, typename K>
	T operator()(const int& n, const int& j, const series_base<T, K>* series) const;
};

template<typename T, typename K>
T u_transform::operator()(const int& n, const int& j, const series_base<T, K>* series) const {
	return 1 / ((BETA + n) * series->operator()(n + j + 1));
}


class t_transform {
public:
	/**
	* @brief Default constructor for t type remainder calculator for S-tranformation, for more information see p. 60 8.4-4 in [https://arxiv.org/pdf/math/0306302.pdf]
	* @authors Venajalainen
	*/
	t_transform() {}

	template<typename T, typename K>
	T operator()(const int& n, const int& j, const series_base<T, K>* series) const;
};

template<typename T, typename K>
T t_transform::operator()(const int& n, const int& j, const series_base<T, K>* series) const {
	return 1 / series->operator()(n + j);
}


class v_transform {
public:
	/**
	* @brief Default constructor for v type remainder calculator for S-tranformation, for more information see p. 60 8.4-5 in [https://arxiv.org/pdf/math/0306302.pdf]
	* @authors Venajalainen
	*/
	v_transform() {}

	template<typename T, typename K>
	T operator()(const int& n, const int& j, const series_base<T, K>* series) const;
};

template<typename T, typename K>
T v_transform::operator()(const int& n, const int& j, const series_base<T, K>* series) const {
	return (series->operator()(n + j + 1) - series->operator()(n + j)) / (series->operator()(n + j + 1) * series->operator()(n + j));
}


template<typename T, typename K, typename series_templ>
class levi_sidi_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	/**
	* @brief Default function to calculate S-tranformation. Implemented u,t and v transformations. For more information see p. 57 8.2-7 [https://arxiv.org/pdf/math/0306302.pdf]
	* Levin-Sidi or Factorial analog of Levin Transformation is effective for series that belong to b(1)/LIN/FAC and inferior on b(1)/LOG for more information see p. 369 and p.285 [http://servidor.demec.ufpr.br/CFD/bibliografia/MER/Sidi_2003.pdf]
	* @authors Venajalainen
	* @param k The number of terms in the partial sum.
	* @param n the order of transformation
	* @param remainder_func functor, whose returning w_n for t,u or v transformation
	* @return The partial sum after the transformation.
	*/

	template<class remainderType>
	T calculate(const K& k, const int& n, remainderType remainder_func) const {

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

			w_n = remainder_func(n, j, this->series);

			numerator += rest * this->series->S_n(n + j) * w_n;
			denominator += rest * w_n;

		}

		numerator /= denominator;

		if (!std::isfinite(numerator))
			throw std::overflow_error("division by zero");

		return numerator;
	}


	template<class remainderType>
	T calculate_recursively(const K& k, const int& n, remainderType remainder_func) const {

		if (n < 0)
			throw std::domain_error("negative integer in input");
		if (BETA <= 0)
			throw std::domain_error("beta cannot be initiared by a negative number or a zero");

		std::vector<T>* N = new std::vector<T>(k + 1, 0);
		std::vector<T>* D = new std::vector<T>(k + 1, 0);

		for (int i = 0; i < k + 1; i++) {
			(*D)[i] = remainder_func(0, n + i, this->series);
			(*N)[i] = this->series->S_n(n + i) * (*D)[i];
		}

		for (int i = 1; i <= k; ++i) {
			for (int j = 0; j <= k - i; ++j) {
				// n' = n + j
				// k = i
				T scale = BETA + n + j + i - 2;
				//std::cout << std::endl << "DENOMINATOR: " << (*D)[j] << " " << (*D)[j + 1] << std::endl;
				(*D)[j] = (scale+i) * (*D)[j + 1] - scale * (*D)[j];
				(*N)[j] = (scale+i) * (*N)[j + 1] - scale * (*N)[j];
			}
		}

		//T numerator = N[0] / D[0];
		T numerator = (*N)[0] / (*D)[0];

		if (!std::isfinite(numerator))
			throw std::overflow_error("division by zero");

		return numerator;
	}


public:

	friend class u_levi_sidi_algorithm<T, K, series_templ>;
	friend class t_levi_sidi_algorithm<T, K, series_templ>;
	friend class v_levi_sidi_algorithm<T, K, series_templ>;
	friend class recursive_u_levi_sidi_algorithm<T, K, series_templ>;
	friend class recursive_t_levi_sidi_algorithm<T, K, series_templ>;
	friend class recursive_v_levi_sidi_algorithm<T, K, series_templ>;


	/**
	* @brief generalised Levi-Sidi class template for derevetions
	* @authors Venajalainen
	* @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
	*/

	levi_sidi_algorithm(const series_templ& series);

	/**
	* @brief Abstract method for the inherited classes below
	* Computes the partial sum after the transformation using the Levin Algorithm.
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @return The partial sum after the transformation.
	*/

	T operator()(const K k, const int n) const = 0;

};

template<typename T, typename K, typename series_templ>
levi_sidi_algorithm< T, K, series_templ>::levi_sidi_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <typename T, typename K, typename series_templ>
class u_levi_sidi_algorithm : public levi_sidi_algorithm<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Levin-Sidi u S-transformation.
	* @authors Venajalainen
	* @param series The series class object to be accelerated
	*/
	u_levi_sidi_algorithm(const series_templ& series);

	/**
	* @brief Impimentation of Levin-Sidi u S-transformation.
	* Computes the partial sum after the transformation using the Levin-Sidi u S-transformation analogues to Levin u L-transformation
	* For more information,
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K k, const int n) const;

};

template<typename T, typename K, typename series_templ>
u_levi_sidi_algorithm< T, K, series_templ> ::u_levi_sidi_algorithm(const series_templ& series) : levi_sidi_algorithm<T, K, series_templ>(series) {}

template<typename T, typename K, typename series_templ>
T u_levi_sidi_algorithm<T, K, series_templ>::operator()(const K k, const int n) const { return levi_sidi_algorithm<T, K, series_templ>::calculate(k, n, u_transform{}); }

template <typename T, typename K, typename series_templ>
class t_levi_sidi_algorithm : public levi_sidi_algorithm<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Levin-Sidi t S-transformation.
	* @authors Venajalainen
	* @param series The series class object to be accelerated
	*/
	t_levi_sidi_algorithm(const series_templ& series);

	/**
	* @brief Impimentation of Levin-Sidi t S-tranformation.
	* Computes the partial sum after the transformation using the Levin-Sidi t S-transformation analogues to Levin t L-transformation
	* t transformation is suitable for strictly alternating series, for more information see p. 43 in [https://arxiv.org/pdf/math/0306302.pdf]
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K k, const int n) const;
};

template<typename T, typename K, typename series_templ>
t_levi_sidi_algorithm < T, K, series_templ> ::t_levi_sidi_algorithm(const series_templ& series) : levi_sidi_algorithm<T, K, series_templ>(series) {}

template<typename T, typename K, typename series_templ>
T t_levi_sidi_algorithm<T, K, series_templ>::operator()(const K k, const int n) const { return levi_sidi_algorithm<T, K, series_templ>::calculate(k, n, t_transform{}); }

template <typename T, typename K, typename series_templ>
class v_levi_sidi_algorithm : public levi_sidi_algorithm<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Levin-Sidi v S-transformation.
	* @authors Venajalainen
	* @param series The series class object to be accelerated
	*/
	v_levi_sidi_algorithm(const series_templ& series);

	/**
	* @brief Impimentation of Levin-Sidi v S-transformation.
	* Computes the partial sum after the transformation using the Levin-Sidi v S-transformation analogues to Levinvt L-transformation
	* for more information see p. 44 in [https://arxiv.org/pdf/math/0306302.pdf]
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @param remainder is parametr to choose the form of a w_i
	* @return The partial sum after the transformation.
	*/
	T operator()(const K k, const int n) const;
};


template<typename T, typename K, typename series_templ>
v_levi_sidi_algorithm < T, K, series_templ> ::v_levi_sidi_algorithm(const series_templ& series) : levi_sidi_algorithm<T, K, series_templ>(series) {}

template<typename T, typename K, typename series_templ>
T v_levi_sidi_algorithm<T, K, series_templ>::operator()(const K k, const int n) const { return levi_sidi_algorithm<T, K, series_templ>::calculate(k, n, v_transform{}); }

template <typename T, typename K, typename series_templ>
class recursive_u_levi_sidi_algorithm : public levi_sidi_algorithm<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Levin-Sidi u S-transformation.
	* @authors Venajalainen
	* @param series The series class object to be accelerated
	*/
	recursive_u_levi_sidi_algorithm(const series_templ& series);

	/**
	* @brief Impimentation of Levin-Sidi u S-transformation.
	* Computes the partial sum after the transformation using the Levin-Sidi u S-transformation analogues to Levin u L-transformation
	* For more information,
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K k, const int n) const;

};

template<typename T, typename K, typename series_templ>
recursive_u_levi_sidi_algorithm< T, K, series_templ> ::recursive_u_levi_sidi_algorithm(const series_templ& series) : levi_sidi_algorithm<T, K, series_templ>(series) {}

template<typename T, typename K, typename series_templ>
T recursive_u_levi_sidi_algorithm<T, K, series_templ>::operator()(const K k, const int n) const { return levi_sidi_algorithm<T, K, series_templ>::calculate_recursively(k, n, u_transform{}); }

template <typename T, typename K, typename series_templ>
class recursive_t_levi_sidi_algorithm : public levi_sidi_algorithm<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Levin-Sidi t S-transformation.
	* @authors Venajalainen
	* @param series The series class object to be accelerated
	*/
	recursive_t_levi_sidi_algorithm(const series_templ& series);

	/**
	* @brief Impimentation of Levin-Sidi t S-tranformation.
	* Computes the partial sum after the transformation using the Levin-Sidi t S-transformation analogues to Levin t L-transformation
	* t transformation is suitable for strictly alternating series, for more information see p. 43 in [https://arxiv.org/pdf/math/0306302.pdf]
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K k, const int n) const;
};

template<typename T, typename K, typename series_templ>
recursive_t_levi_sidi_algorithm < T, K, series_templ> ::recursive_t_levi_sidi_algorithm(const series_templ& series) : levi_sidi_algorithm<T, K, series_templ>(series) {}

template<typename T, typename K, typename series_templ>
T recursive_t_levi_sidi_algorithm<T, K, series_templ>::operator()(const K k, const int n) const { return levi_sidi_algorithm<T, K, series_templ>::calculate_recursively(k, n, t_transform{}); }

template <typename T, typename K, typename series_templ>
class recursive_v_levi_sidi_algorithm : public levi_sidi_algorithm<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Levin-Sidi v S-transformation.
	* @authors Venajalainen
	* @param series The series class object to be accelerated
	*/
	recursive_v_levi_sidi_algorithm(const series_templ& series);

	/**
	* @brief Impimentation of Levin-Sidi v S-transformation.
	* Computes the partial sum after the transformation using the Levin-Sidi v S-transformation analogues to Levinvt L-transformation
	* for more information see p. 44 in [https://arxiv.org/pdf/math/0306302.pdf]
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @param remainder is parametr to choose the form of a w_i
	* @return The partial sum after the transformation.
	*/
	T operator()(const K k, const int n) const;
};


template<typename T, typename K, typename series_templ>
recursive_v_levi_sidi_algorithm < T, K, series_templ> ::recursive_v_levi_sidi_algorithm(const series_templ& series) : levi_sidi_algorithm<T, K, series_templ>(series) {}

template<typename T, typename K, typename series_templ>
T recursive_v_levi_sidi_algorithm<T, K, series_templ>::operator()(const K k, const int n) const { return levi_sidi_algorithm<T, K, series_templ>::calculate_recursively(k, n, v_transform{}); }