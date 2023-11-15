/**
 * @file shanks_transform.h
 * @brief This file contains the definition of the Shanks transformation class.
 */

#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series.h" // Include the series header
#include <vector>  // Include the vector library

 /**
  * @brief Shanks transformation class.
  * @tparam T The type of the elements in the series.
  */
template <typename T>
class shanks_transform : public series_acceleration<T>
{
public:

	/**
   * @brief Default constructor.
   */
	shanks_transform();

	/**
   * @brief Parameterized constructor to initialize the Shanks transformation.
   * @param series The series function to be accelerated.
   * @param x The value of x.
   */
	shanks_transform(const std::function<T(const T, const int)>& series, const T x);

	/**
   * @brief Destructor to clean up resources.
   */
	~shanks_transform() override;
private:
	/**
   * @brief Shanks transformation function.
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @return The partial sum after the transformation.
   */
	T transform(const int n, const int order) const override;
};

/**
 * @brief Default constructor for the Shanks transformation.
 * Initializes the Shanks transformation.
 */
template <typename T>
shanks_transform<T>::shanks_transform() : series_acceleration<T>()
{

}

/**
 * @brief Parameterized constructor for the Shanks transformation.
 * Initializes the Shanks transformation with a series function and a value of x.
 * @param series The series function to be accelerated.
 * @param x The value of x.
 */
template <typename T>
shanks_transform<T>::shanks_transform(const std::function<T(T, int)>& series, const T x) : series_acceleration<T>(series, x)
{

}

/**
 * @brief Destructor to clean up resources for the Shanks transformation.
 */
template <typename T>
shanks_transform<T>::~shanks_transform()
{

}

/**
 * @brief Shanks transformation function.
 * Transforms the partial sum based on the number of terms and the order of transformation.
 * @param n The number of terms in the partial sum.
 * @param order The order of transformation.
 * @return The partial sum after the transformation.
 */
template <typename T>
T shanks_transform<T>::transform(const int n, const int order) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	else if (order == 0) /*it is convenient to assume that transformation of order 0 is no transformation at all*/
		return this->S_n(n);
	else if (n < order || n == 0)
		return DEF_UNDEFINED_SUM;
	else if (order == 1)
	{
		const auto a_n = this->series(this->x, n);
		const auto a_n_plus_1 = this->series(this->x, n + 1);
		if (isnan(abs(a_n - a_n_plus_1)))
			throw std::overflow_error("division by zero");
		return std::fma(a_n * a_n_plus_1, 1 / (a_n - a_n_plus_1), this->S_n(n));
	}
	else //n > order >= 1
	{
		std::vector<T> T_n(n + order, 0);
		for (int i = n - order + 1; i <= n + order - 1; ++i) // if we got to this branch then we know that n >= order - see previous branches
		{
			const auto a_n = this->series(this->x, i);
			const auto a_n_plus_1 = this->series(this->x, i + 1);
			if (isnan(abs(a_n - a_n_plus_1)))
				throw std::overflow_error("division by zero");
			T_n[i] = std::fma(a_n * a_n_plus_1, 1 / (a_n - a_n_plus_1), this->S_n(i));
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
				if (isnan(abs(2 * a - b - c)))
					throw std::overflow_error("division by zero");
				/*if (isnan(abs(2 * T_n[i] - T_n[i - 1] - T_n[i + 1])))
					throw std::overflow_error("division by zero");*/
					/*T_n_plus_1[i] = T_n[i] - (T_n[i] - T_n[i - 1]) * (T_n[i + 1] - T_n[i]) / (T_n[i + 1] - 2 * T_n[i] + T_n[i - 1]);
					T_n_plus_1[i] = std::fma(std::fma(T_n[i], T_n[i+1] + T_n[i-1] - T_n[i], -T_n[i-1]*T_n[i+1]), 1 / (2 * T_n[i] - T_n[i - 1] - T_n[i+1]), T_n[i]);*/
				T_n_plus_1[i] = std::fma(std::fma(a, c + b - a, -b * c), 1 / (2 * a - b - c), a);
				T_n = T_n_plus_1;
			}
		}
		return T_n[n];
	}
}