#ifndef COS3XMIB1_DIV_XSQUARE_ITERATOR_HPP
#define COS3XMIB1_DIV_XSQUARE_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file cos3xmin1_div_xsquare_iterator.hpp
 * @brief Iterator for the series expansion of (cos(3x-1))/x^2.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = (cos(3x-1))/x^2.
 *
 * This class implements the expansion of the shifted cosine function normalized
 * by x^2. The series is singular at x=0.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class cos3xmin1_div_xsquare_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for cos3xmin1_div_xsquare_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	cos3xmin1_div_xsquare_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series ((cos(3x-1))/x^2).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of (cos(3x-1))/x^2.
     */
	T sum() const override{ return utils::cos(utils::cast<T>(3)*this->x-utils::cast<T>(1)) / (this->x * this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is zero or non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x) || this->x == utils::cast<T>(0); }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {
		// First term corresponds to the leading term of the expansion at n=0
		if (this->n == 0) this->current_state = utils::cast<T>(1) / (this->x * this->x);
		else{
			const T var_3x_1squared = utils::fma(utils::cast<T>(3),this->x,utils::cast<T>(-1)) *
									  utils::fma(utils::cast<T>(3),this->x,utils::cast<T>(-1));
			// Recursive update based on the Taylor series for cos(u) where u = 3x-1
			this->current_state *= utils::cast<T>(-1) * var_3x_1squared /
			utils::cast<T>(2*this->n*utils::fma(size_t{2},this->n-1,size_t{1}));
		}

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
