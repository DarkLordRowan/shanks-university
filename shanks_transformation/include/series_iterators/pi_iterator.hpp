#ifndef PI_ITERATOR_HPP
#define PI_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

/**
* @brief Maclaurin series of x * pi function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class pi_iterator final : public series_base_iterator<T, K>{
public:

	pi_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{ return this->x * utils::cast<T>(std::numbers::pi);}
	
	bool check_validity() const override { return !utils::isfinite(this->x); }

	T next() override {

		if (this->n == 0) this->current_state = this->x * utils::sqrt(utils::cast<T>(12));
		else this->current_state *= utils::cast<T>(-1) * utils::cast<T>(utils::fma(size_t{2},this->n-1,size_t{1})) / 
		utils::cast<T>(3 * utils::fma(size_t{2},this->n,size_t{1}));
		
		this->n += 1;
		return this->current_state;
	}

};

#endif