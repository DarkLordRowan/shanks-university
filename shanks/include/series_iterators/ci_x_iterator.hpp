#ifndef CI_X_ITERATOR_HPP
#define CI_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file ci_x_iterator.hpp
 * @brief Iterator for the series expansion of the Cosine Integral Ci(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the Cosine Integral function Ci(x).
 *
 * This class implements the expansion of Ci(x) around zero, which includes
 * logarithmic and constant terms (Euler-Mascheroni constant) followed by
 * an infinite power series.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class ci_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for ci_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	ci_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (Ci(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of Ci(x).
     */
	T sum() const override{ return utils::ci_x(this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the Ci(x) expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// First few terms are special (constant and logarithmic), followed by the power series
		if (this->n == 0) this->current_state = utils::cast<T>(std::numbers::egamma);
		else if (this->n == 1) this->current_state = utils::log(this->x);
		else if (this->n == 2) this->current_state = utils::cast<T>(-0.25) * this->x * this->x;
		else this->current_state *= utils::cast<T>(-1) * this->x * this->x / utils::cast<T>(2*(this->n-1)*(this->n-1)*utils::fma(size_t{2},this->n-2,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
