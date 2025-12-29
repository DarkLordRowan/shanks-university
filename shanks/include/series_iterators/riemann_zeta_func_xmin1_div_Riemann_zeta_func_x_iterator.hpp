#ifndef RIEMANN_ZETA_FUNC_XMIN1_DIV_RIEMANN_ZETA_FUNC_X_ITERATOR_HPP
#define RIEMANN_ZETA_FUNC_XMIN1_DIV_RIEMANN_ZETA_FUNC_X_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of dzeta(z-1)/dzeta(z) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator final : public series_base_iterator<T, K>{
public:

	riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::zeta(this->x - utils::cast<T>(1.0)) / utils::zeta(this->x);}

	bool check_validity() const override {

		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval

		if constexpr (isComplexLike<T>::value){
    		return !utils::isfinite(this->x) || this->x.real() <= utils::cast<float_type>(2);
		} else {
			return !utils::isfinite(this->x) || this->x <= utils::cast<T>(2);
		}

	}

	T next() override {
		
		this->current_state = utils::phi<T, K>(this->n+1) / utils::pow(utils::cast<T>(this->n+1), this->x);
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif