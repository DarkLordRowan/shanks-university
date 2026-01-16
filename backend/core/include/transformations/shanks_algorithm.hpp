#ifndef SHANKS_ALGORITHM_HPP
#define SHANKS_ALGORITHM_HPP
#pragma once
/**
 * @file shanks_algorithm.hpp
 * @brief This file contains the definition of the Shanks transformation class
 *        and specialization of this transformation for alternating series.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

 // For theory, see:
 // Shanks, D. (1955). Non-linear transformations of divergent and slowly convergent sequences.
 // Journal of Mathematics and Physics, 34(1-4), 1-42.
 // Senhadji, M.N. (2001). On condition numbers of the Shanks transformation.
 // Journal of Computational and Applied Mathematics, 135(1), 41-61.
 // Brezinski, C., He, Y., Hu, X.B., Redivo-Zaglia, M., & Sun, J.Q. (2010).
 // Multistep epsilon-algorithm, Shanks' transformation, and Lotka-Volterra system by Hirota's method.
 // Mathematics of Computation.

#include "series_acceleration.hpp"

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Core>

#ifdef EIGEN_CORE_MODULE_H
namespace Eigen {
/**
 * @brief Template specialization of NumTraits for mpfr::mpreal to support Eigen integration.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
template<> struct NumTraits<mpfr::mpreal>
 : NumTraits<double> // permits to get the epsilon, dummy_precision, lowest, highest functions
{
  typedef mpfr::mpreal Real;
  typedef mpfr::mpreal NonInteger;
  typedef mpfr::mpreal Nested;

  enum {
    IsComplex = 0,
    IsInteger = 0,
    IsSigned = 1,
    RequireInitialization = 1,
    ReadCost = 1,
    AddCost = 3,
    MulCost = 3
  };
};
}
#endif

namespace shanks{ namespace algos{

/**
 * @brief Shanks transformation for series class template.
 *
 * Implements the general Shanks transformation using its determinant representation.
 * This nonlinear sequence transformation is highly effective for accelerating the
 * convergence of sequences where terms behave like a sum of exponentials.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam T Floating-point type for series elements (must satisfy AcceptedLike).
 *           Represents numerical precision (float, double, long double, or arbitrary precision).
 * @tparam K Unsigned integral type for indices and order (must satisfy UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class shanks_algorithm final : public series_acceleration<T, K>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Shanks transformation for non-alternating series.
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	*/
	explicit shanks_algorithm() : series_acceleration<T, K>("shanks original") {};

	/**
	 * @brief Executes the general determinant-based Shanks transformation.
	 *
	 * Computes the accelerated estimate using the ratio of two determinants.
	 * The transformation order determines the size of the matrices used in the calculation.
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param n The number of terms in the partial sum to use for transformation
	 *        Valid values: n >= order > 0, n > 0
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (number of recursive applications)
	 *        Valid values: order >= 0 (order=0 returns untransformed partial sum)
	 *        Higher orders provide more aggressive acceleration but may be less stable
	 * @param data series_result<T> struct containing necessary information for algorithm
	 * @return T The accelerated partial sum result.
	 * @throws std::out_of_range if the Sn vector size is insufficient for the requested order.
	 * @throws std::overflow_error if division by zero or numerical instability occurs.
	 */
	T operator()(
		const K n,
        const K order,
        const series_result<T>& data
	) const;
};

template <AcceptedLike T, UnsignedIntLike K>
T shanks_algorithm<T, K>::operator()(
	const K n,
    const K order,
    const series_result<T>& data
) const{

    // Validation: required size is 2*order + n + 1
    const K required_size = static_cast<K>(2) * order + n + static_cast<K>(1);
	const size_t precision = utils::get_precision(data.Sn[0]);

    if (data.Sn.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for shanks_{" + utils::to_string(order) + "}^{" + utils::to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + utils::to_string(required_size));
	}

	//create matrix for further determinant calculation
	const size_t matrix_size = order + static_cast<K>(1);
	::Eigen::Matrix<T, ::Eigen::Dynamic, ::Eigen::Dynamic> matrix_template; matrix_template.resize(matrix_size, matrix_size);

	T upper_determinant, lower_determinant;
	upper_determinant = lower_determinant = utils::cast<T>(0, precision);

	// Fill the common part of the matrices (rows 1 to order) with partial sum differences
	for (size_t row = 1; row < matrix_size; ++row) for(size_t col = 0; col < matrix_size; ++col){
		if constexpr (is_precisable<T>::value) utils::set_precision(utils::get_precision(data.Sn[0]), matrix_template(row,col));
		matrix_template(row,col) = data.Sn[n + col + row] - data.Sn[n + col];
	}

	// Compute the upper determinant by filling the first row with partial sums
	for (size_t col = 0; col < matrix_size; ++col){
		if constexpr (is_precisable<T>::value) utils::set_precision(utils::get_precision(data.Sn[0]), matrix_template(0,col));
		matrix_template(0,col) = data.Sn[n + col];
	}
	upper_determinant += matrix_template.determinant();

	// Compute the lower determinant by filling the first row with ones
	for (size_t col = 0; col < matrix_size; ++col){
		matrix_template(0,col) = utils::cast<T>(1, precision);

	}
	lower_determinant += matrix_template.determinant();

    // Final ratio yields the accelerated value
	const T result = upper_determinant / lower_determinant;

	if (!utils::isfinite(result)) throw std::overflow_error("division by zero");

	return result;
}

} //namespace shanks::algos
} //namespace shanks

#endif
