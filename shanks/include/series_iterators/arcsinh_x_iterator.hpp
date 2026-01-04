#ifndef ARCSINH_X_ITERATOR_HPP
#define ARCSINH_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file arcsinh_x_iterator.hpp
 * @brief Iterator for the arcsinh(x) Taylor series expansion.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the inverse hyperbolic sine function arcsinh(x).
 *
 * This class implements the Maclaurin expansion of arcsinh(x), which converges
 * for values of x within the range [-1, 1].
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class arcsinh_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for arcsinh_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	arcsinh_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (arcsinh(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of arcsinh(x).
     */
	T sum() const override{ return utils::asinh(this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| > 1 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the arcsinh(x) Maclaurin expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Initialize with the first term x or calculate subsequent terms recursively
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= utils::cast<T>(-1) * this->x * this->x *
		utils::cast<T>(utils::fma(size_t{2},this->n-1,size_t{1})) / utils::cast<T>(size_t{2} * this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
