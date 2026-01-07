#ifndef HALF_MULTI_LN_1DIV2MULTI1MISUSCOSX_ITERATOR_HPP
#define HALF_MULTI_LN_1DIV2MULTI1MISUSCOSX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file half_multi_ln_1div2multi1minuscosx_iterator.hpp
 * @brief Iterator for the series expansion of -0.5 * ln(2 - 2*cos(x)).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Fourier series iterator for the function f(x) = -0.5 * ln(2 - 2*cos(x)).
 *
 * This class implements the Fourier series for the logarithmic function, which
 * converges for all x except multiples of 2*pi.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class half_multi_ln_1div2multi1minuscosx_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for half_multi_ln_1div2multi1minuscosx_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	half_multi_ln_1div2multi1minuscosx_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (-0.5 * ln(2 - 2*cos(x))).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of -0.5 * ln(2 - 2*cos(x)).
     */
	T sum() const override{ return utils::cast<T>(-0.5) * utils::log(utils::cast<T>(2) - utils::cast<T>(2) * utils::cos(this->x)); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= pi or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(std::numbers::pi);
	}

    /**
     * @brief Computes the next term in the Fourier expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Fourier series term: cos((n+1)x) / (n+1)
		this->current_state = utils::cos(utils::cast<T>(this->n+1) * this->x) / utils::cast<T>(this->n+1);
		this->n+=1;

		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
