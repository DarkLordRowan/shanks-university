/**
 * @file shanks_transform.h
 * @brief This file contains the definition of the Shanks transformation class.
 */

#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration.h" // Include the series header
#include <vector>  // Include the vector library

 /**
  * @brief Shanks transformation class.
  * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
  */
template <typename T, typename K, typename series_templ>
class shanks_transform : public series_acceleration<T, K, series_templ>
{
public:

	/**
   * @brief Default constructor.
   * @author Bolshakov M.P.
   */
	shanks_transform();

	/**
   * @brief Parameterized constructor to initialize the Shanks transformation.
   * @authors Bolshakov M.P.
   * @param series The series class object
   */
	shanks_transform(const series_templ& series);

	/**
   * @brief Destructor to clean up resources.
   * @authors Bolshakov M.P.
   */
	~shanks_transform() override;
	/**
   * @brief Shanks transformation function.
   * @authors Bolshakov M.P., Pashkov B.B.
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @return The partial sum after the transformation.
   */
	T operator()(const K n, const int order) const;
};

template <typename T, typename K, typename series_templ>
shanks_transform<T, K, series_templ>::shanks_transform() : series_acceleration<T, K, series_templ>()
{

}

template <typename T, typename K, typename series_templ>
shanks_transform<T, K, series_templ>::shanks_transform(const series_templ& series) : series_acceleration<T, K, series_templ>(series)
{

}

template <typename T, typename K, typename series_templ>
shanks_transform<T, K, series_templ>::~shanks_transform()
{

}

template <typename T, typename K, typename series_templ>
T shanks_transform<T, K, series_templ>::operator()(const K n, const int order) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	else if (order == 0) /*it is convenient to assume that transformation of order 0 is no transformation at all*/
		return this->series.S_n(n);
	else if (n < order || n == 0)
		return DEF_UNDEFINED_SUM;
	else if (order == 1)
	{
		const auto a_n = this->series.a_n(n);
		const auto a_n_plus_1 = this->series.a_n(n + 1);
		if (!std::isfinite(abs(a_n - a_n_plus_1)))
			throw std::overflow_error("division by zero");
		const auto tmp = -a_n_plus_1 * a_n_plus_1;
		return std::fma(a_n * a_n_plus_1, (a_n + a_n_plus_1) / (std::fma(a_n, a_n, tmp) - std::fma(a_n_plus_1, a_n_plus_1, tmp)), this->series.S_n(n));
	}
	else //n > order >= 1
	{
		std::vector<T> T_n(n + order, 0);
		auto a_n = this->series.a_n(n - order);
		auto a_n_plus_1 = this->series.a_n(n - order + 1);
		for (int i = n - order + 1; i <= n + order - 1; ++i) // if we got to this branch then we know that n >= order - see previous branches
		{
			a_n = this->series.a_n(i);
			a_n_plus_1 = this->series.a_n(i + 1);
			if (!std::isfinite(abs(a_n - a_n_plus_1)))
				throw std::overflow_error("division by zero");
			const auto tmp = -a_n_plus_1 * a_n_plus_1;

			// formula [6]
			T_n[i] = std::fma(a_n * a_n_plus_1, (a_n + a_n_plus_1) / (std::fma(a_n, a_n, tmp) - std::fma(a_n_plus_1, a_n_plus_1, tmp)), this->series.S_n(i));
		}
		std::vector<T> T_n_plus_1(n + order, 0);
		T a, b, c;
		for (int j = 2; j <= order; ++j)
		{
			for (int i = n - order + j; i <= n + order - j; ++i)
			{
				a = T_n[i];
				b = T_n[i - 1];
				c = T_n[i + 1];
				if (!std::isfinite(abs(2 * a - b - c)))
					throw std::overflow_error("division by zero");
				/*if (!std::isfinite(abs(2 * T_n[i] - T_n[i - 1] - T_n[i + 1])))
					throw std::overflow_error("division by zero");*/
					/*T_n_plus_1[i] = T_n[i] - (T_n[i] - T_n[i - 1]) * (T_n[i + 1] - T_n[i]) / (T_n[i + 1] - 2 * T_n[i] + T_n[i - 1]);
					T_n_plus_1[i] = std::fma(std::fma(T_n[i], T_n[i+1] + T_n[i-1] - T_n[i], -T_n[i-1]*T_n[i+1]), 1 / (2 * T_n[i] - T_n[i - 1] - T_n[i+1]), T_n[i]);*/
				T_n_plus_1[i] = std::fma(std::fma(a, c + b - a, -b * c), 1 / (2 * a - b - c), a); 
			}
			T_n = T_n_plus_1;
		}
		return T_n[n];
	}
}