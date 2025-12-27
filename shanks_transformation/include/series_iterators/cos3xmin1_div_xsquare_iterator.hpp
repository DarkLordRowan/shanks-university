#ifndef COS3XMIB1_DIV_XSQUARE_ITERATOR_HPP
#define COS3XMIB1_DIV_XSQUARE_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of cos(x^2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class cos3xmin1_div_xsquare_iterator final : public series_base_iterator<T, K>{
public:

	cos3xmin1_div_xsquare_iterator() : series_base_iterator<T, K>() {}

	T sum() const override{ return utils::cos(utils::cast<T>(3)*this->x-utils::cast<T>(1)) / (this->x * this->x);}
	
	bool check_validity() const override { return !utils::isfinite(this->x) || this->x == utils::cast<T>(0); }

	T next() override {
		if (this->n == 0) this->current_state = utils::cast<T>(1) / (this->x * this->x);
		else{
			const T var_3x_1squared = utils::fma(utils::cast<T>(3),this->x,utils::cast<T>(-1)) * 
									  utils::fma(utils::cast<T>(3),this->x,utils::cast<T>(-1));
			this->current_state *= utils::cast<T>(-1) * var_3x_1squared / 
			utils::cast<T>(2*this->n*utils::fma(size_t{2},this->n-1,size_t{1}));
		}

		this->n+=1;
		return this->current_state;
	}

};

#endif