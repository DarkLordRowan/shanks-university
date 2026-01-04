#ifndef TEN_MINUS_X_ITERATOR_HPP
#define TEN_MINUS_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file ten_minus_x_iterator.hpp
 * @brief Iterator for the Fourier series expansion of the linear function f(x) = 10 - x.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Fourier series iterator for the linear function f(x) = 10 - x.
 *
 * This class implements a specific Fourier expansion for the function 10 - x,
 * which is valid for values of x such that |x - 10| <= 5.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class ten_minus_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for ten_minus_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	ten_minus_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (10 - x).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 10 - x.
     */
	T sum() const override{ return utils::cast<T>(10) - this->x;}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x - 10| > 5 or x is non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x - utils::cast<T>(10)) > utils::cast<float_type>(5);
	}

    /**
     * @brief Computes the next term in the Fourier expansion of 10 - x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {
		// Specific Fourier term formula for the linear segment expansion
		this->current_state = utils::cast<T>(10) * utils::minus_one_raised_to_power_n<T, K>(this->n+1) /
		(utils::cast<T>(this->n+1) * utils::cast<T>(std::numbers::pi)) * utils::sin(utils::cast<T>(this->n+1) * utils::cast<T>(std::numbers::pi * 0.2) * this->x);
		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
