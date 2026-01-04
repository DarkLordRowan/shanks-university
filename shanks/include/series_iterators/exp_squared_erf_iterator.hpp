#ifndef EXP_SQUARED_ERF_ITERATOR_HPP
#define EXP_SQUARED_ERF_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"
#include <numbers>

/**
 * @file exp_squared_erf_iterator.hpp
 * @brief Iterator for the Taylor series expansion of exp(x^2)*erf(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Taylor series iterator for the function f(x) = exp(x^2) * erf(x).
 *
 * This class implements the power series expansion of the product of the
 * Gaussian-like exponential and the error function.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class exp_squared_erf_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for exp_squared_erf_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	exp_squared_erf_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (exp(x^2) * erf(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the product exp(x^2) * erf(x).
     */
	T sum() const override{ return utils::erf(this->x) * utils::exp(this->x * this->x);}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool check_validity() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the exp(x^2) * erf(x) expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next() override {

		// Recurrence relation for the power series expansion of exp(x^2)*erf(x)
		if (this->n == 0) this->current_state = utils::cast<T>(2) * this->x / utils::sqrt(utils::cast<T>(std::numbers::pi));
		else this->current_state *= utils::cast<T>(2) * this->x * this->x / utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}));

		this->n+=1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
