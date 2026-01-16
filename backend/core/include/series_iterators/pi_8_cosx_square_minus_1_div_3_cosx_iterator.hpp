#ifndef PI_8_COSX_SQUARE_MINUS_1_DIV_3_COSX_ITERATOR_HPP
#define PI_8_COSX_SQUARE_MINUS_1_DIV_3_COSX_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"
#include <numbers>

/**
 * @file pi_8_cosx_square_minus_1_div_3_cosx_iterator.hpp
 * @brief Iterator for the series expansion related to (pi/8)*cos^2(x) - cos(x)/3.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Fourier-like series iterator for the trigonometric function f(x) = (pi/8)*cos^2(x) - cos(x)/3.
 *
 * This class implements a specific expansion whose sum involves squared and linear
 * cosine terms, typically valid for x in [-pi/2, pi/2].
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class pi_8_cosx_square_minus_1_div_3_cosx_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for pi_8_cosx_square_minus_1_div_3_cosx_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	explicit pi_8_cosx_square_minus_1_div_3_cosx_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of (pi/8)*cos^2(x) - cos(x)/3.
     */
	T get_sum() const override{
		return utils::cast<T>(std::numbers::pi * 0.125) * utils::cos(this->x) * utils::cos(this->x) - utils::cos(this->x) / utils::cast<T>(3);
	}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| > pi/2 or non-finite, false otherwise.
     */
	bool is_invalid() const override {
		using float_type = GetUnderlyingType<T>::value;
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(std::numbers::pi * 0.5);
	}

    /**
     * @brief Computes the next term in the trigonometric series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next(K n, T& state) const override {

		// General term formula involving alternating higher-order cosine harmonics
		state = utils::minus_one_raised_to_power_n<T, K>(n) * utils::cos(utils::cast<T>(utils::fma(static_cast<size_t>(2),static_cast<size_t>(n),static_cast<size_t>(3))) * this->x) /
		utils::cast<T>(utils::fma(static_cast<size_t>(2),static_cast<size_t>(n),static_cast<size_t>(1)) * utils::fma(static_cast<size_t>(2),static_cast<size_t>(n),static_cast<size_t>(3)) * utils::fma(static_cast<size_t>(2),static_cast<size_t>(n),static_cast<size_t>(5)));
		return state;
	}

};

}} //namespace shanks
#endif
