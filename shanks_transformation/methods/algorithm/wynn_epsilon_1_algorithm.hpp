/**
 * @file wynn_epsilon_1_algorithm.hpp
 * @brief This file contains the declaration of the Epsilon Algorithm class.
 */

#include "../series_acceleration.hpp"
#include <vector> // Include the vector library

 /**
  * @brief Epsilon Algorithm class template.
  * @tparam T Element type of the series.
  * @tparam K Integer index type.
  * @tparam series_templ Series type to accelerate.
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class wynn_epsilon_1_algorithm final : public series_acceleration<T, K, series_templ>
{
public:
	/**
     * @brief Parameterized constructor to initialize the Epsilon Algorithm.
     * @authors Pashkov B.B.
     * @param series The series class object to be accelerated
     */
	explicit wynn_epsilon_1_algorithm(const series_templ& series);

	/**
     * @brief Shanks multistep epsilon algorithm.
     * Computes the partial sum after the transformation using the Epsilon Algorithm.
     * For more information, see p. 5.3.2 in [https://e-maxx.ru/bookz/files/numerical_recipes.pdf]
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */
    T operator()(K n, K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
wynn_epsilon_1_algorithm<T, K, series_templ>::wynn_epsilon_1_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T wynn_epsilon_1_algorithm<T, K, series_templ>::operator()(const K n, const K order) const
{

	using std::isfinite;

	if (order == static_cast<K>(0))
		return this->series->S_n(n);

	const K m = static_cast<K>(2) * order;
	K max_ind = m + n; // int -> K mark

	const K n1 = n - static_cast<K>(1);

	std::vector<T> e0(max_ind + static_cast<K>(1), static_cast<T>(0));
	std::vector<T> e1(max_ind                    , static_cast<T>(0));

	auto e0_add = &e0; // Pointer for vector swapping
	auto e1_add = &e1; // Pointer for vector swapping

	K j = max_ind;
	do {
		e0[j] = this->series->S_n(j);
	} while (--j > static_cast<K>(0));

	for (K i = static_cast<K>(0); i < m; ++i) {
		for (K j = n1; j < max_ind; ++j)
			(*e1_add)[j] += static_cast<T>(1) / ((*e0_add)[j + static_cast<K>(1)] - (*e0_add)[j]);

		--max_ind;
		std::swap(e0_add, e1_add);
		(*e1_add).erase((*e1_add).begin());
	}

	if (!isfinite((*e0_add)[n1]))
		throw std::overflow_error("division by zero");

	return (*e0_add)[n1];
}