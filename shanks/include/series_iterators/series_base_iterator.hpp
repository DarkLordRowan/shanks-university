#ifndef SERIES_BASE_ITERATOR_HPP
#define SERIES_BASE_ITERATOR_HPP
#pragma once

#include "../custom_concepts.hpp"
#include "../utils.hpp"

namespace shanks { namespace iters {

template <AcceptedLike T, UnsignedIntLike K>
class series_base_iterator{
public:

    T x;
    T current_state;
    size_t n;

    series_base_iterator() : x(0.0), current_state(0.0), n(0) {}

    series_base_iterator(const T& x, const size_t step_forward = 0) : x(x), n(step_forward){
        #ifdef SET_PRECISION_SET
        if constexpr (!is_standard_types<T>::value) utils::set_precision(utils::get_precision<T>(x), x, current_state);
        #endif
        step(step_forward);
    }

    virtual T sum() const = 0;
    virtual bool check_validity() const = 0;

    virtual T next() = 0;
    T step(const size_t n_steps){ for(size_t j = 0; j < n_steps; ++j) next(); return next(); }

    void reset(){n = static_cast<size_t>(0);}

};

} //namespace shanks::iters
} //namespace shanks

#endif