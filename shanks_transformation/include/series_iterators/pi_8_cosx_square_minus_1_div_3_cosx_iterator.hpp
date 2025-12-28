#ifndef PI_8_COSX_SQUARE_MINUS_1_DIV_3_COSX_ITERATOR_HPP
#define PI_8_COSX_SQUARE_MINUS_1_DIV_3_COSX_ITERATOR_HPP
#pragma once

#include "series_base_iterator.hpp"

namespace shanks { namespace iters {

/**
* @brief Maclaurin series of pi/8*cos(x)^2-cos(x)/3 function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template<AcceptedLike T, UnsignedIntLike K>
class pi_8_cosx_square_minus_1_div_3_cosx_iterator final : public series_base_iterator<T, K>{
public:

	pi_8_cosx_square_minus_1_div_3_cosx_iterator() : series_base_iterator<T, K>() {}
	
	T sum() const override{
		return utils::cast<T>(std::numbers::pi * 0.125) * utils::cos(this->x) * utils::cos(this->x) - utils::cos(this->x) / utils::cast<T>(3);
	}
	
	bool check_validity() const override {
		using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval
		return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(std::numbers::pi * 0.5);
	}

	T next() override {
	
		this->current_state = utils::minus_one_raised_to_power_n<T, K>(this->n) * utils::cos(utils::cast<T>(utils::fma(size_t{2},this->n,size_t{3})) * this->x) / 
		utils::cast<T>(utils::fma(size_t{2},this->n,size_t{1}) * utils::fma(size_t{2},this->n,size_t{3}) * utils::fma(size_t{2},this->n,size_t{5}));
		this->n += 1;
		return this->current_state;
	}

};

} //namespace shanks::iters
} //namespace shanks

#endif