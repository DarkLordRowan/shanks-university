#ifndef HALF_MINUS_SINX_MULTI_PI_4_ITERATOR_HPP
#define HALF_MINUS_SINX_MULTI_PI_4_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file half_minus_sinx_multi_pi_4_iterator.hpp
 * @brief Iterator for the series expansion of 0.5 - (pi/4)*sin(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Series iterator for the function f(x) = 0.5 - (pi/4) * sin(x).
 *
 * This class implements a specific series expansion for the given trigonometric function,
 * typically valid for x within the range [0, pi/2].
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class half_minus_sinx_multi_pi_4_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for half_minus_sinx_multi_pi_4_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	half_minus_sinx_multi_pi_4_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (0.5 - (pi/4)*sin(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 0.5 - (pi/4)*sin(x).
     */
	T sum() const override{ return utils::cast<T>(0.5) - utils::cast<T>(std::numbers::pi * 0.25) * utils::sin(this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is outside [0, pi/2] or non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		if constexpr (isComplexLike<T>::value){
    		return !utils::isfinite(this->x) || this->x.real() < utils::cast<float_type>(0) || this->x.real() > utils::cast<float_type>(0.5 * std::numbers::pi);
		} else {
			return !utils::isfinite(this->x) || this->x < utils::cast<T>(0) || this->x > utils::cast<T>(0.5 * std::numbers::pi);
		}
	}

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {
		// Specific term formula for the expansion of 0.5 - (pi/4)*sin(x)
		this->current_state = utils::cos(utils::cast<T>(utils::fma(size_t{2},this->n,size_t{2})) * this->x) /
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{3}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
