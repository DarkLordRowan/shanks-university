/**
 * @file epsilon_algorithm.h
 * @brief This file contains the declaration of the Epsilon Algorithm class.
 */

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library

 /**
  * @brief Epsilon Algorithm class template.
  * @tparam T Element type of the series.
  * @tparam K Integer index type.
  * @tparam series_templ Series type to accelerate.
  */
template <typename T, typename K, typename series_templ>
class epsilon_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
     * @brief Parameterized constructor to initialize the Epsilon Algorithm.
     * @authors Pashkov B.B.
     * @param series The series class object to be accelerated
     */
	epsilon_algorithm(const series_templ& series);

	/**
     * @brief Shanks multistep epsilon algorithm.
     * Computes the partial sum after the transformation using the Epsilon Algorithm.
     * For more information, see p. 5.3.2 in [https://e-maxx.ru/bookz/files/numerical_recipes.pdf]
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */
    T operator()(const K n, const int order) const;
};

template <typename T, typename K, typename series_templ>
epsilon_algorithm<T, K, series_templ>::epsilon_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <typename T, typename K, typename series_templ>
T epsilon_algorithm<T, K, series_templ>::operator()(const K n, const int order) const
{
	if (n <= 0)
		throw std::domain_error("negative integer in the input");

	if (order == 0)
		return this->series->S_n(n);

	const int m = 2 * order;
	K max_ind = m + n; // int -> K mark

	//TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения
	const K n1 = n - 1;

	std::vector<T> e0(max_ind + 1, 0);
	std::vector<T> e1(max_ind,     0);

	auto e0_add = &e0; // Pointer for vector swapping
	auto e1_add = &e1; // Pointer for vector swapping
	for (int j = int(max_ind); j >= 0; --j)
		e0[j] = this->series->S_n(j);

	for (int i = 0; i < m; ++i)
	{
		for (K j = n1; j < max_ind; ++j)
			(*e1_add)[j] += static_cast<T>(1.0 / ((*e0_add)[j + 1] - (*e0_add)[j]));

		--max_ind;
		std::swap(e0_add, e1_add);
		(*e1_add).erase((*e1_add).begin());
	}

	if (!std::isfinite((*e0_add)[n1]))
		throw std::overflow_error("division by zero");

	return (*e0_add)[n1];
}