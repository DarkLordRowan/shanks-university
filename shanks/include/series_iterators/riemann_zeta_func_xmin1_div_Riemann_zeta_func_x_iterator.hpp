#ifndef RIEMANN_ZETA_FUNC_XMIN1_DIV_RIEMANN_ZETA_FUNC_X_ITERATOR_HPP
#define RIEMANN_ZETA_FUNC_XMIN1_DIV_RIEMANN_ZETA_FUNC_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
 * @file riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator.hpp
 * @brief Iterator for the series representation of zeta(s-1) / zeta(s).
 * @authors Bolshakov M.P.
 */

namespace shanks { namespace iters {

/**
 * @brief Dirichlet series iterator for the ratio of Riemann zeta functions zeta(s-1) / zeta(s).
 * 
 * This class implements the Dirichlet series expansion for the ratio of zeta functions, 
 * which involves the Euler totient function phi(n). It converges for values of s such 
 * that Re(s) > 2.
 * 
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator final : public series_base_iterator<T, K>{
public:

    /**
     * @brief Default constructor for riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator.
     * @authors Bolshakov M.P.
     */
	riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator() : series_base_iterator<T, K>() {}

    /**
     * @brief Retrieves the analytic sum of the series (zeta(s-1) / zeta(s)).
     * @authors Bolshakov M.P.
     * @return T The value of the ratio zeta(s-1) / zeta(s).
     */
	T sum() const override{ return utils::zeta(this->x - utils::cast<T>(1.0)) / utils::zeta(this->x);}
	
    /**
     * @brief Validates the current evaluation point x (s).
     * @authors Bolshakov M.P.
     * @return true if Re(x) <= 2 or x is non-finite, false otherwise.
     */
	bool check_validity() const override {

		using float_type = GetUnderlyingType<T>::value;

		if constexpr (isComplexLike<T>::value){
    		return !utils::isfinite(this->x) || this->x.real() <= utils::cast<float_type>(2);
		} else {
			return !utils::isfinite(this->x) || this->x <= utils::cast<T>(2);
		}

	}

    /**
     * @brief Computes the next term in the Dirichlet series for zeta(s-1)/zeta(s).
     * @authors Bolshakov M.P.
     * @return T The next term of the series (phi(n+1) / (n+1)^s).
     */
	T next() override {
		
		// Dirichlet series term involving Euler's totient function
		this->current_state = utils::phi<T, K>(this->n+1) / utils::pow(utils::cast<T>(this->n+1), this->x);
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif