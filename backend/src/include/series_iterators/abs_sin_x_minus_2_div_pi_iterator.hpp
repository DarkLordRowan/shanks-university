#ifndef ABS_SIN_X_MINUS_2_DIV_PI_ITERATOR_HPP
#define ABS_SIN_X_MINUS_2_DIV_PI_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file abs_sin_x_minus_2_div_pi_iterator.hpp
 * @brief Iterator for the Fourier series expansion of |sin(x)| - 2/pi.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Fourier series iterator for the function f(x) = |sin(x)| - 2/pi.
 *
 * This class computes terms of the Fourier series for the periodic absolute
 * sine function, shifted by its mean value (2/pi).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class abs_sin_x_minus_2_div_pi_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for abs_sin_x_minus_2_div_pi_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	abs_sin_x_minus_2_div_pi_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series at the current point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of |sin(x)| - 2/pi.
     */
	T sum() const override{

		if constexpr (isComplexLike<T>::value){
			// Check periodicity/branch for complex numbers using the real part
			if (this->x.real() <= utils::cast<T>(std::numbers::pi).real())
				return utils::sin(this->x) - utils::cast<T>(2) / utils::cast<T>(std::numbers::pi);
		} else {
			// Standard real case
			if (this->x <= utils::cast<T>(std::numbers::pi))
           		return utils::sin(this->x) - utils::cast<T>(2) / utils::cast<T>(std::numbers::pi);
		}

		// Negative branch for absolute value
		return utils::cast<T>(-1) * utils::sin(this->x) - utils::cast<T>(2) / utils::cast<T>(std::numbers::pi);
	}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is outside [0, 2pi] or non-finite, false otherwise.
     */
	bool check_validity() const override {

		if constexpr (isComplexLike<T>::value){
			return !utils::isfinite(this->x) || this->x.real() < utils::cast<T>(0).real() || this->x.real() > utils::cast<T>(2.0 * std::numbers::pi).real();
		} else {
           	return !utils::isfinite(this->x) || this->x < utils::cast<T>(0) || this->x > utils::cast<T>(2.0 * std::numbers::pi);
		}

	}

    /**
     * @brief Computes the next term in the Fourier expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Formula for the n-th term of the |sin(x)| Fourier series
		this->current_state = utils::cast<T>(-4) * utils::cos(utils::cast<T>(utils::fma(size_t{2},this->n,size_t{2}))*this->x) /
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{3})) / utils::cast<T>(std::numbers::pi);

		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
