#ifndef ONE_TWELFTH_ITERATOR_HPP
#define ONE_TWELFTH_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of 1/12*(3*x^2-pi^2) function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class one_twelfth_iterator final : public series_base_iterator<T, K>{
public:

	one_twelfth_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return this->x / utils::cast<T>(12);}
	
	bool check_validity() const override { return !utils::isfinite(this->x);}

	T next() override {
		
		this->current_state = this->x / utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) *
		utils::fma(size_t{2},this->n,size_t{3}) * utils::fma(size_t{2},this->n,size_t{5}));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif