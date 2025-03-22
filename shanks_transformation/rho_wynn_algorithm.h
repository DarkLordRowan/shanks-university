/**
 * @file rho-wynn_algorithm.h
 * @brief This file contains the declaration of the Rho Wynn Algorithm class.
 * @authors Yurov P.I. Bezzaborov A.A.
 */

#pragma once
#define DEF_UNDEFINED_SUM 0
#define GAMMA 2 //parameter for gamma modification
#define RHO 1	//parameter for gamma-rho modification

#include "series_acceleration.h" // Include the series header


 /**
  * @brief Rho Wynn Algorithm class template.
  * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
  */
template <typename T, typename K, typename series_templ>
class rho_Wynn_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	const numerator_base<T, K>* numerator_func;

	T calculate(const K n, int order) const { //const int order
		if (order % 2 != 0) {
			//throw std::domain_error("order should be even number");
			std::cout << "order should be even number. order increased by 1" << std::endl;
			order++;
		}
		if (n < 0 || order < -2)
			throw std::domain_error("negative integer in the input");
		else if (order == 0) {
			return this->series->S_n(n);
		}
		T S_n = this->series->S_n(n);
		T res = recursive_calculate_body(n, order - 2, S_n,1) + (numerator_func->operator()(n,order,this->series, GAMMA, RHO)) / (recursive_calculate_body(n, order - 1,S_n,1) - recursive_calculate_body(n, order - 1,S_n,0));

		if (!std::isfinite(res)) throw std::overflow_error("division by zero");
		return res;
	}

	T recursive_calculate_body(const K n, const int order, T S_n, const K j) const {
		/** 
		* S_n - previous sum;
		* j - adjusts n: (n + j);
		*/
		S_n += (j == K(0)) ? T(0) : this->series->operator()(n + j);
		if (order == 0) {
			return S_n;
		}
		else if (order == -1) {
			return 0;
		}
		T res = recursive_calculate_body(n + j, order - 2, S_n, 1) + (numerator_func->operator()(n+j,order,this->series, GAMMA, RHO)) / (recursive_calculate_body(n + j, order - 1, S_n, 1) - recursive_calculate_body(n + j, order - 1, S_n, 0));
		if (!std::isfinite(res)) throw std::overflow_error("division by zero");
		return res;
	}
public:
	/**
   * @brief Parameterized constructor to initialize the Rho Wynn Algorithm.
   * @param series The series class object to be accelerated
   */
	rho_Wynn_algorithm(const series_templ& series, const numerator_base<T,K>* func) : series_acceleration<T, K, series_templ>(series) {
		if (func == nullptr) throw std::domain_error("null poniter numerator function");
		numerator_func = func;
	}
	
	~rho_Wynn_algorithm() { if(numerator_func != nullptr)delete numerator_func; }
	/**
   * @brief Rho Wynn algorithm.
   * Computes the partial sum after the transformation using the Rho Wynn Algorithm.
   * For more information, see 
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @return The partial sum after the transformation.
   */
	T operator()(const K n, const int order) const
	{
		return calculate(n, order);
	}
};