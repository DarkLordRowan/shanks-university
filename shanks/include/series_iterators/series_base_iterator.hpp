#ifndef SERIES_BASE_ITERATOR_HPP
#define SERIES_BASE_ITERATOR_HPP
#pragma once

/**
 * @file series_base_iterator.hpp
 * @brief This file contains the base class for series iterators, defining the common interface.
 * @authors Bolshakov M.P.
 */

#include "../custom_concepts.hpp"
#include "../utils.hpp"

namespace shanks { namespace iters {

/**
 * @brief Abstract base class for series iterators.
 *
 * Provides a common foundation for iterating through terms of a mathematical series.
 * It manages the current state, the point of evaluation (x), and the number of steps taken.
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (must satisfy AcceptedLike).
 * @tparam K Unsigned integral type for indexing (must satisfy UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class series_base_iterator{
public:

    T x;               /**< The point at which the series is evaluated. */
    T current_state;   /**< The current value of the term or partial sum. */
    size_t n;          /**< The current index or step number in the iteration. */

    /**
     * @brief Default constructor for series_base_iterator.
     * @authors Bolshakov M.P.
     */
    series_base_iterator() : x(0.0), current_state(0.0), n(0) {}

    /**
     * @brief Parameterized constructor to initialize evaluation point and starting step.
     *
     * @authors Bolshakov M.P.
     * @param x The evaluation point.
     * @param step_forward The number of initial steps to advance the iterator.
     */
    series_base_iterator(const T& x, const size_t step_forward = 0) : x(x), n(step_forward){
        #ifdef SET_PRECISION_SET
        // Adjust precision if the type T is not a standard primitive type
        if constexpr (!is_standard_types<T>::value) utils::set_precision(utils::get_precision<T>(x), x, current_state);
        #endif
        // Advance the iterator to the requested starting position
        step(step_forward);
    }

    /**
     * @brief Retrieves the current cumulative sum of the series.
     * @authors Bolshakov M.P.
     * @return T The sum of terms computed so far.
     */
    virtual T sum() const = 0;

    /**
     * @brief Validates if the current evaluation point x is within the convergence radius.
     * @authors Bolshakov M.P.
     * @return true if valid, false otherwise.
     */
    virtual bool check_validity() const = 0;

    /**
     * @brief Advances to the next term in the series and updates current_state.
     * @authors Bolshakov M.P.
     * @return T The value of the next term.
     */
    virtual T next() = 0;

    /**
     * @brief Advances the iterator by multiple steps.
     *
     * @authors Bolshakov M.P.
     * @param n_steps Number of steps to advance.
     * @return T
     */
    T step(const size_t n_steps){
        for(size_t j = 0; j < n_steps; ++j) next();
        return next();
    }

    /**
     * @brief Resets the iteration counter to zero.
     * @authors Bolshakov M.P.
     */
    void reset(){n = static_cast<size_t>(0);}

};

} //namespace shanks::iters
} //namespace shanks

#endif
