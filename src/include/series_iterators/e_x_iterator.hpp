#ifndef E_X_ITERATOR_HPP
#define E_X_ITERATOR_HPP

#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file e_x_iterator.hpp
 * @brief Iterator for the series expansion of the E_x(x) function.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the complete elliptic integral related function E_x(x).
 *
 * This class implements a specific series expansion for E_x(x), which converges
 * for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class e_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for e_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	e_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (E_x(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of E_x(x).
     */
	T sum() const override{ return utils::e_x(this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
	}

    /**
     * @brief Computes the next term in the E_x(x) expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// First term is pi/2, subsequent terms use the specialized recurrence for E_x
		if (this->n == 0) this->current_state = utils::cast<T>(std::numbers::pi * 0.5);
		else this->current_state *= this->x * this->x * (utils::cast<T>((this->n-1)*(this->n-1))-utils::cast<T>(0.25))/utils::cast<T>(this->n*this->n);

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
