#ifndef SERIES_WITH_LN_NUMBER1_ITERATOR_HPP
#define SERIES_WITH_LN_NUMBER1_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file series_with_ln_number1_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * 0.599195688977.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series {

/**
 * @brief Series iterator for a complex expansion summing to x * 0.599195688977.
 *
 * This class implements a specific series expansion whose analytic sum equals
 * approximately 0.599195688977 * x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class series_with_ln_number1_iterator final : public series_base_succ<T, K>{
public:

    /**
     * @brief Default constructor for series_with_ln_number1_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
	explicit series_with_ln_number1_iterator(T x) : series_base_succ<T, K>(x) {
	    if (this->is_invalid())
			throw std::invalid_argument("Invalid series argument");
	}

    /**
     * @brief Retrieves the approximate analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 0.599195688977 * x.
     */
	T get_sum() const override{ return utils::cast<T>(0.599195688977) * this->x;}

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
	bool is_invalid() const override { return !utils::isfinite(this->x); }

    /**
     * @brief Computes the next term in the specific expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
	T next(K n, T& state) const override {
		const T n1 = utils::cast<T>(n + 1);
		const T n1_2 = n1 * n1;

		// Specific term formula involving factorials, exponentials, and logarithms
		state = utils::log(utils::cast<T>(1) +
		utils::pow(n1, n1_2 + n1 * utils::cast<T>(0.5)) / (utils::pow(utils::cast<T>(utils::fact<K>(n+1)), n1) * utils::exp(n1_2))) * this->x;
		return state;
	}

};

}} //namespace shanks
#endif
