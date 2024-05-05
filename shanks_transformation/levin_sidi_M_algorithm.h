/**
* @file levin_sidi_M_algorithm.h
* @brief This files contains the definition of analogues of Levin-Sidi S-transformation
*/
#pragma once
#define DEF_UNDEFINED_SUM 0
#define GAMMA 10 // gamma is a a nonzero positive parameter, 10 is chosen by default


#include "series_acceleration.h" // Include the series header
#include <vector>
#include <iostream>

template<typename T, typename K, typename series_templ> class alevi_sidi_algorithm;

template<typename T, typename K, typename series_templ> class u_alevi_sidi_algorithm;

template<typename T, typename K, typename series_templ> class t_alevi_sidi_algorithm;

template<typename T, typename K, typename series_templ> class d_alevi_sidi_algorithm;

template<typename T, typename K, typename series_templ> class v_alevi_sidi_algorithm;


class a_u_transform {
public:
	/**
	* @brief Default constructor for u type remainder calculator for M-tranformation, for more information see p. 67 9.4-1 in [https://arxiv.org/pdf/math/0306302.pdf]
	* @authors Yurov P.I. Bezzaborov A.A.
	*/
	a_u_transform() {}

	template<typename T, typename K>
	T operator()(const int& n, const int& j, const series_base<T, K>* series) const;
};

template<typename T, typename K>
T a_u_transform::operator()(const int& n, const int& j, const series_base<T, K>* series) const {
	return 1 / ((-GAMMA - n) * series->operator()(n + j + 1)); 
}

class a_t_transform {
public:
	/**
	* @brief Default constructor for t type remainder calculator for M-tranformation, for more information see p. 43 7.3-6 in [https://arxiv.org/pdf/math/0306302.pdf]
	* @authors Yurov P.I. Bezzaborov A.A.
	*/
	a_t_transform() {}

	template<typename T, typename K>
	T operator()(const int& n, const int& j, const series_base<T, K>* series) const;
};

template<typename T, typename K>
T a_t_transform::operator()(const int& n, const int& j, const series_base<T, K>* series) const {
	return 1 / (series->operator()(n + j));
}

class a_d_transform {
public:
	/**
	* @brief Default constructor for d type remainder calculator for M-tranformation, for more information see p. 43 7.3-8 in [https://arxiv.org/pdf/math/0306302.pdf]
	* @authors Yurov P.I. Bezzaborov A.A.
	*/
	a_d_transform() {}

	template<typename T, typename K>
	T operator()(const int& n, const int& j, const series_base<T, K>* series) const;
};

template<typename T, typename K>
T a_d_transform::operator()(const int& n, const int& j, const series_base<T, K>* series) const {
	return 1 / (series->operator()(n + j+1));
}

class a_v_transform {
public:
	/**
	* @brief Default constructor for v type remainder calculator for M-tranformation, for more information see p. 43 7.3-10 in [https://arxiv.org/pdf/math/0306302.pdf]
	* @authors Yurov P.I. Bezzaborov A.A.
	*/
	a_v_transform() {}

	template<typename T, typename K>
	T operator()(const int& n, const int& j, const series_base<T, K>* series) const;
};

template<typename T, typename K>
T a_v_transform::operator()(const int& n, const int& j, const series_base<T, K>* series) const {
	return (series->operator()(n + j) - series->operator()(n + j + 1)) / (series->operator()(n + j) * series->operator()(n + j + 1)) ;
}

template<typename T, typename K, typename series_templ>
class alevi_sidi_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	/**
	* @brief Default function to calculate M-tranformation. Implemented u,t,d and v transformations. For more information see p. 65 9.2-6 [https://arxiv.org/pdf/math/0306302.pdf]
	* Levin-Sidi or Factorial analog of Levin Transformation is effective for series that belong to b(1)/LIN/FAC and inferior on b(1)/LOG for more information see p. 369 and p.285 [http://servidor.demec.ufpr.br/CFD/bibliografia/MER/Sidi_2003.pdf]
	* @authors Yurov P.I. Bezzaborov A.A.
	* @param k The number of terms in the partial sum.
	* @param n the order of transformation
	* @param remainder_func functor, whose returning w_n for u,t,d or v transformation
	* @return The partial sum after the transformation.
	* The Pochhammer symbol satisfies (-x)_n = (-1)^n*(x-n+1)_n
	*/

	template<class remainderType>
	T calculate(const K& k, const int& n, remainderType remainder_func) const {

		if (n < 0)
			throw std::domain_error("negative integer in input");
		if (GAMMA <= k-1)
			throw std::domain_error("gamma cannot be lesser than k-1");


		T numerator = T(0), denominator = T(0);
		T w_n, rest;
		T up, down;

		T binomial_coef = this->series->binomial_coefficient(k, 0);
		T S_n = this->series->S_n(n);

		up = down = T(1);
		for (int m = 0; m < k - 1; ++m) {
			up *= (GAMMA + n - k + 2 + m);
			down *= (GAMMA + n + 2 + m);
		}

		for (int j = 0; j <= k; ++j) {

			rest = this->series->minus_one_raised_to_power_n(j) * binomial_coef;
			binomial_coef = binomial_coef * (k - j) / (j + 1);

			rest = rest * (up / down);

			up = up / (GAMMA + n + j - k + 2) * (GAMMA + n + j + 1);

			w_n = remainder_func(n, j, this->series);

			numerator += rest * S_n * w_n;

			S_n += this->series->operator()(n + j + 1);

			denominator += rest * w_n;

		}

		numerator /= denominator;

		if (!std::isfinite(numerator))
			throw std::overflow_error("division by zero");

		return numerator;
	}


public:

	friend class u_alevi_sidi_algorithm<T, K, series_templ>;
	friend class t_alevi_sidi_algorithm<T, K, series_templ>;
	friend class v_alevi_sidi_algorithm<T, K, series_templ>;

	/**
	* @brief generalised Levi-Sidi class template for derivations
	* @authors Yurov P.I. Bezzaborov A.A.
	* @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
	*/

	alevi_sidi_algorithm(const series_templ& series);

	/**
	* @brief Abstract method for the inherited classes below
	* Computes the partial sum after the transformation using the Levin Algorithm.
	* For, more information about u,t,d and d transformations see p. 67-68 9.4-1 - 9.4-5
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @return The partial sum after the transformation.
	*/

	T operator()(const K k, const int n) const = 0;

};

template<typename T, typename K, typename series_templ>
alevi_sidi_algorithm< T, K, series_templ>::alevi_sidi_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}


template <typename T, typename K, typename series_templ>
class u_alevi_sidi_algorithm : public alevi_sidi_algorithm<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Levin-Sidi u M-transformation.
	* @authors Yurov P.I. Bezzaborov A.A.
	* @param series The series class object to be accelerated
	*/
	u_alevi_sidi_algorithm(const series_templ& series);

	/**
	* @brief Impimentation of Levin-Sidi u M-transformation.
	* Computes the partial sum after the transformation using the Levin-Sidi u M-transformation
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K k, const int n) const;

};

template<typename T, typename K, typename series_templ>
u_alevi_sidi_algorithm< T, K, series_templ> ::u_alevi_sidi_algorithm(const series_templ& series) : alevi_sidi_algorithm<T, K, series_templ>(series) {}

template<typename T, typename K, typename series_templ>
T u_alevi_sidi_algorithm<T, K, series_templ>::operator()(const K k, const int n) const { return alevi_sidi_algorithm<T, K, series_templ>::calculate(k, n, a_u_transform{}); }


template <typename T, typename K, typename series_templ>
class t_alevi_sidi_algorithm : public alevi_sidi_algorithm<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Levin-Sidi t M-transformation.
	* @authors Yurov P.I. Bezzaborov A.A.
	* @param series The series class object to be accelerated
	*/
	t_alevi_sidi_algorithm(const series_templ& series);

	/**
	* @brief Impimentation of Levin-Sidi t M-transformation.
	* Computes the partial sum after the transformation using the Levin-Sidi t M-transformation
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K k, const int n) const;

};

template<typename T, typename K, typename series_templ>
t_alevi_sidi_algorithm< T, K, series_templ> ::t_alevi_sidi_algorithm(const series_templ& series) : alevi_sidi_algorithm<T, K, series_templ>(series) {}

template<typename T, typename K, typename series_templ>
T t_alevi_sidi_algorithm<T, K, series_templ>::operator()(const K k, const int n) const { return alevi_sidi_algorithm<T, K, series_templ>::calculate(k, n, a_t_transform{}); }


template <typename T, typename K, typename series_templ>
class d_alevi_sidi_algorithm : public alevi_sidi_algorithm<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Levin-Sidi d M-transformation.
	* @authors Yurov P.I. Bezzaborov A.A.
	* @param series The series class object to be accelerated
	*/
	d_alevi_sidi_algorithm(const series_templ& series);

	/**
	* @brief Impimentation of Levin-Sidi d M-transformation.
	* Computes the partial sum after the transformation using the Levin-Sidi d M-transformation
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K k, const int n) const;

};

template<typename T, typename K, typename series_templ>
d_alevi_sidi_algorithm< T, K, series_templ> ::d_alevi_sidi_algorithm(const series_templ& series) : alevi_sidi_algorithm<T, K, series_templ>(series) {}

template<typename T, typename K, typename series_templ>
T d_alevi_sidi_algorithm<T, K, series_templ>::operator()(const K k, const int n) const { return alevi_sidi_algorithm<T, K, series_templ>::calculate(k, n, a_d_transform{}); }

template <typename T, typename K, typename series_templ>
class v_alevi_sidi_algorithm : public alevi_sidi_algorithm<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Levin-Sidi v M-transformation.
	* @authors Yurov P.I. Bezzaborov A.A.
	* @param series The series class object to be accelerated
	*/
	v_alevi_sidi_algorithm(const series_templ& series);

	/**
	* @brief Impimentation of Levin-Sidi v M-transformation.
	* Computes the partial sum after the transformation using the Levin-Sidi v M-transformation
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K k, const int n) const;

};

template<typename T, typename K, typename series_templ>
v_alevi_sidi_algorithm< T, K, series_templ> ::v_alevi_sidi_algorithm(const series_templ& series) : alevi_sidi_algorithm<T, K, series_templ>(series) {}

template<typename T, typename K, typename series_templ>
T v_alevi_sidi_algorithm<T, K, series_templ>::operator()(const K k, const int n) const { return alevi_sidi_algorithm<T, K, series_templ>::calculate(k, n, a_v_transform{}); }