#ifndef RIEMANN_ZETA_FUNC_ITERATOR_HPP
#define RIEMANN_ZETA_FUNC_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file riemann_zeta_func_iterator.hpp
 * @brief Iterator for the Dirichlet series defining the Riemann zeta function zeta(s).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace iters {

/**
 * @brief Dirichlet series iterator for the Riemann zeta function zeta(s).
 *
 * This class implements the standard Dirichlet series representation of zeta(s),
 * which converges for complex values of s such that Re(s) > 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class riemann_zeta_func_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for riemann_zeta_func_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	riemann_zeta_func_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (zeta(s)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the Riemann zeta function at point x (s).
     */
	T sum() const override{ return utils::zeta(this->x);}

    /**
     * @brief Validates the current evaluation point x (s).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if Re(x) <= 1 or x is non-finite, false otherwise.
     */
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value;

		if constexpr (isComplexLike<T>::value){
    		return !utils::isfinite(this->x) || this->x.real() <= utils::cast<float_type>(1);
		} else {
			return !utils::isfinite(this->x) || this->x <= utils::cast<T>(1);
		}

	}

    /**
     * @brief Computes the next term in the Dirichlet series for zeta(s).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series ((n+1)^(-s)).
     */
	T next() override {

		// Term formula: 1 / (n+1)^s
		this->current_state = utils::pow(utils::cast<T>(this->n+1), utils::cast<T>(-1) * this->x);
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif
