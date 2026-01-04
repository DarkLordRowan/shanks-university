#ifndef COS_SQRT_X_ITERATOR_HPP
#define COS_SQRT_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file cos_sqrt_x_iterator.hpp
 * @brief Iterator for the series expansion of cos(sqrt(x)).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = cos(sqrt(x)).
 *
 * This class implements the expansion of cos(sqrt(x)), which converges for
 * x in the range [0, +inf) for real values. Note that this effectively
 * becomes a power series in x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class cos_sqrt_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for cos_sqrt_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	cos_sqrt_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (cos(sqrt(x))).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of cos(sqrt(x)).
     */
	T sum() const override{ return utils::cos(utils::sqrt(this->x)); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite or negative, false otherwise.
     */
	bool check_validity() const override {

		if constexpr (isComplexLike<T>::value){
    		return !utils::isfinite(this->x) || this->x.real() <= utils::cast<T>(0).real();
		} else {
			return !utils::isfinite(this->x) || this->x < utils::cast<T>(0);
		}

	}

    /**
     * @brief Computes the next term in the cos(sqrt(x)) series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {
		// First term is 1.0, subsequent terms derived from the cos Taylor expansion with u = sqrt(x)
		if (this->n == 0) this->current_state = utils::cast<T>(1);
		else this->current_state *= utils::cast<T>(-1) * this->x  / utils::cast<T>(2*this->n*utils::fma(size_t{2},this->n-1,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
