#ifndef ARCSIN_X_ITERATOR_HPP
#define ARCSIN_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file arcsin_x_iterator.hpp
 * @brief Iterator for the Taylor series expansion of arcsin(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the inverse sine function arcsin(x).
 *
 * This class implements the Maclaurin expansion of arcsin(x), which converges
 * for values of x such that |x| <= 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class arcsin_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for arcsin_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	arcsin_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (arcsin(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of arcsin(x).
     */
	T sum() const override{ return utils::asin(this->x);}

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
     * @brief Computes the next term in the arcsin(x) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Standard Taylor expansion recursion: each term is derived from the previous one
		if (this->n == 0) this->current_state = this->x;
		else this->current_state *= this->x * this->x *
		utils::cast<T>(utils::fma(size_t{2},this->n-1,size_t{1}) * utils::fma(size_t{2},this->n-1,size_t{1})) /
		utils::cast<T>(2 * this->n * utils::fma(size_t{2},this->n,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
