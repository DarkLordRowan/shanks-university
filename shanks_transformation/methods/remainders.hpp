/**
 * @file transform_base.h
 * @brief This file contains the declaration of base classes for remainder estimates
 *        used in Levin-type sequence transformations.
 *
 * For theory, see:
 * Levin, D. (1973). Development of non-linear transformations for improving convergence of sequences.
 * Sidi, A., & Levin, D. (1982). Rational approximations from the d-transformation.
 * Homeier, H.H.H., & Weniger, E.J. (1995). On remainder estimates for Levin-type sequence transformations.
 */

#pragma once

#include <concepts>
#include "../series.h"

 /**
  * @brief Enum for remainder types to use in Levin-type transformations
  *
  * Defines different variants of remainder estimates for Levin transformations:
  * - u_variant: ωₙ = (n+β)ΔSₙ₋₁ (for series acceleration)
  * - t_variant: ωₙ = ΔSₙ (term difference)
  * - v_variant: ωₙ = (ΔSₙ₋₁ΔSₙ)/(ΔSₙ - ΔSₙ₋₁) (modified Aitken δ²-process)
  * - t_wave_variant: ωₙ = ΔSₙ₊₁ (forward-shifted term difference)
  * - v_wave_variant: ωₙ = (ΔSₙΔSₙ₊₁)/(ΔSₙ - ΔSₙ₊₁) (shifted v-variant)
  *
  * @authors Bolshakov M.P.
  * @edited by Kreynin R.G.
  */
enum remainder_type{
         u_variant,
         t_variant,
         v_variant,
    t_wave_variant,
    v_wave_variant,
};

/**
 * @brief Abstract base class for remainder estimates in Levin-type transformations
 *
 * Provides the interface for different remainder estimate variants used in
 * Levin's sequence transformations for convergence acceleration.
 *
 * @tparam T Floating-point type for series elements (must satisfy FloatLike)
 *           Represents numerical precision (float, double, long double)
 * @tparam K Unsigned integral type for indices (must satisfy std::unsigned_integral)
 *           Used for counting and indexing operations
 */
template<FloatLike T, std::unsigned_integral K>
class transform_base{
public:

    /**
     * @brief Virtual operator() for computing remainder estimates
     *
     * Computes the remainder estimate ωₙ for Levin-type transformations.
     * The specific formula depends on the concrete implementation.
     *
     * @param n Index of the partial sum Sₙ from which calculation begins
     * @param order Order of the transformation (number of terms used)
     * @param series Pointer to the series object providing term access
     * @param scale Scaling factor (used primarily for u-variant with β parameter)
     * @return The computed remainder estimate ωₙ
     * @throws std::overflow_error if division by zero or numerical instability occurs
     */
    virtual T operator() (K n, K order, const series::series_base<T,K>* series, T scale = static_cast<T>(1)) const = 0;
};

/**
 * @brief Class for u-variant remainder estimate (ωₙ = (n+β)ΔSₙ₋₁)
 *
 * Implements the u-variant of Levin's transformation remainder estimate.
 * For theory, see: Levin (1973), Eq. (3.3) - u transform
 *
 * @tparam T Floating-point type for series elements
 * @tparam K Unsigned integral type for indices
 */
template<FloatLike T, std::unsigned_integral K>
class u_transform : public transform_base<T, K> {

    /**
     * @brief Computes u-variant remainder estimate: ωₙ = (n+β)ΔSₙ₋₁
     *
     * For theory, see: Levin (1973), Eq. (3.3)
     * Formula: ωₙ = (β + n) * aₙ, where aₙ = ΔSₙ₋₁ = Sₙ - Sₙ₋₁
     *
     * @param n Index of the partial sum Sₙ
     * @param order Order of transformation (unused in this implementation)
     * @param series Series object providing term access
     * @param scale Represents β parameter: ωₙ = (scale + n) * aₙ
     * @return u-variant remainder estimate ωₙ = 1/[(scale + n) * aₙ]
     * @throws std::overflow_error if aₙ = 0 causing division by zero
     */
    T operator() (K n, K order, const series::series_base<T,K>* series, T scale = static_cast<T>(1)) const override;
};

template<FloatLike T, std::unsigned_integral K>
T u_transform<T, K>::operator()(const K n, const K order, const series::series_base<T,K>* series, T scale) const {

    using std::isfinite;

    // For theory, see: Levin (1973), Eq. (3.3) - u transform
    // ωₙ = (β + n) * aₙ, where aₙ = ΔSₙ₋₁
    const T result = static_cast<T>(1) / (scale * series->operator()(n+order));

    if (!isfinite(result)) throw std::overflow_error("division by zero");

    return result;
 }

/**
 * @brief Class for t-variant remainder estimate (ωₙ = ΔSₙ)
 *
 * Implements the t-variant of Levin's transformation remainder estimate.
 * For theory, see: Levin (1973), Eq. (3.2) - t transform
 *
 * @tparam T Floating-point type for series elements
 * @tparam K Unsigned integral type for indices
 */
template<FloatLike T, std::unsigned_integral K>
class t_transform : public transform_base<T, K> {

    /**
     * @brief Computes t-variant remainder estimate: ωₙ = ΔSₙ
     *
     * For theory, see: Levin (1973), Eq. (3.2)
     * Formula: ωₙ = aₙ, where aₙ = ΔSₙ₋₁ = Sₙ - Sₙ₋₁
     *
     * @param n Index of the partial sum Sₙ
     * @param order Order of transformation (determines which term aₙ₊ₖ is used)
     * @param series Series object providing term access
     * @param scale Unused parameter (maintained for interface consistency)
     * @return t-variant remainder estimate ωₙ = 1/aₙ₊ₖ
     * @throws std::overflow_error if aₙ₊ₖ = 0 causing division by zero
     */
    T operator() (K n, K order, const series::series_base<T,K>* series, T scale = static_cast<T>(1)) const override;
};

template<FloatLike T, std::unsigned_integral K>
T t_transform<T, K>::operator()(const K n, const K order, const series::series_base<T,K>* series, T scale) const {

    using std::isfinite;

    // For theory, see: Levin (1973), Eq. (3.2) - t transform
    // ωₙ = aₙ, where aₙ = ΔSₙ₋₁
    const T result = static_cast<T>(1) / series->operator()(n+order);
    if (!isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}

/**
 * @brief Class for t-wave variant remainder estimate (ωₙ = ΔSₙ₊₁)
 *
 * Implements a shifted t-variant remainder estimate (also known as d-variant).
 * For theory, see: Smith & Ford (1979), Eq. (2.4) - d variant
 *
 * @tparam T Floating-point type for series elements
 * @tparam K Unsigned integral type for indices
 */
template<FloatLike T, std::unsigned_integral K>
class t_wave_transform : public transform_base<T, K>  {

    /**
     * @brief Computes t-wave variant remainder estimate: ωₙ = ΔSₙ₊₁
     *
     * For theory, see: Smith & Ford (1979), Eq. (2.4) - d variant
     * Formula: ωₙ = aₙ₊₁, where aₙ = ΔSₙ₋₁
     *
     * @param n Index of the partial sum Sₙ
     * @param order Order of transformation (shifts the index further)
     * @param series Series object providing term access
     * @param scale Unused parameter (maintained for interface consistency)
     * @return t-wave variant remainder estimate ωₙ = 1/aₙ₊ₖ₊₁
     * @throws std::overflow_error if aₙ₊ₖ₊₁ = 0 causing division by zero
     */
    T operator() (K n, K order, const series::series_base<T,K>* series, T scale = static_cast<T>(1)) const override;
};

template<FloatLike T, std::unsigned_integral K>
T t_wave_transform<T,K>::operator()(const K n, const K order, const series::series_base<T, K>* series, T scale ) const {

    using std::isfinite;

    // For theory, see: Smith & Ford (1979), Eq. (2.4) - d variant
    // ωₙ = aₙ₊₁ (shifted t-variant)
	const T result = static_cast<T>(1) / series->operator()(n + order + static_cast<K>(1));

	if (!isfinite(result)) throw std::overflow_error("division by zero");
	return result;
}

/**
 * @brief Class for v-variant remainder estimate (ωₙ = (ΔSₙ₋₁ΔSₙ)/(ΔSₙ - ΔSₙ₋₁))
 *
 * Implements the v-variant of Levin's transformation remainder estimate.
 * For theory, see: Levin (1973), Eq. (3.4) - v transform
 *
 * @tparam T Floating-point type for series elements
 * @tparam K Unsigned integral type for indices
 */
template<FloatLike T, std::unsigned_integral K>
class v_transform : public transform_base<T, K> {

    /**
     * @brief Computes v-variant remainder estimate: ωₙ = (ΔSₙ₋₁ΔSₙ)/(ΔSₙ - ΔSₙ₋₁)
     *
     * For theory, see: Levin (1973), Eq. (3.4) - v transform
     * Formula: ωₙ = (aₙ * aₙ₊₁)/(aₙ₊₁ - aₙ), where aₙ = ΔSₙ₋₁
     *
     * @param n Index of the partial sum Sₙ
     * @param order Order of transformation (determines starting index)
     * @param series Series object providing term access
     * @param scale Unused parameter (maintained for interface consistency)
     * @return v-variant remainder estimate ωₙ = (aₙ₊ₖ₊₁ - aₙ₊ₖ)/(aₙ₊ₖ * aₙ₊ₖ₊₁)
     * @throws std::overflow_error if aₙ₊ₖ = 0 or aₙ₊ₖ₊₁ = 0 causing division by zero
     */
    T operator() (K n, K order, const series::series_base<T,K>* series, T scale = T(1)) const override;
};

template<FloatLike T, std::unsigned_integral K>
T v_transform<T,K>::operator()(const K n, const K order, const series::series_base<T,K>* series, T scale) const {

    using std::isfinite;

    // For theory, see: Levin (1973), Eq. (3.4) - v transform
    // ωₙ = (aₙ * aₙ₊₁)/(aₙ₊₁ - aₙ)
    const T a1 = series->operator()(n+order), a2  = series->operator()(n+order+static_cast<K>(1));
    const T result = (a2-a1) / (a1 * a2);
	if (!isfinite(result)) throw std::overflow_error("division by zero");
	return result;
}

/**
 * @brief Class for v-wave variant remainder estimate (ωₙ = (ΔSₙΔSₙ₊₁)/(ΔSₙ - ΔSₙ₊₁))
 *
 * Implements a shifted v-variant remainder estimate.
 * Similar to v-variant but with indices shifted by one position.
 *
 * @tparam T Floating-point type for series elements
 * @tparam K Unsigned integral type for indices
 */
template<FloatLike T, std::unsigned_integral K>
class v_wave_transform : public transform_base<T, K> {
    
    /**
     * @brief Computes v-wave variant remainder estimate: ωₙ = (ΔSₙΔSₙ₊₁)/(ΔSₙ - ΔSₙ₊₁)
     *
     * Similar to v-variant but with shifted indices:
     * Formula: ωₙ = (aₙ₊₁ * aₙ₊₂)/(aₙ₊₁ - aₙ₊₂)
     *
     * @param n Index of the partial sum Sₙ
     * @param order Order of transformation (determines starting index)
     * @param series Series object providing term access
     * @param scale Unused parameter (maintained for interface consistency)
     * @return v-wave variant remainder estimate ωₙ = (aₙ₊ₖ - aₙ₊ₖ₊₁)/(aₙ₊ₖ * aₙ₊ₖ₊₁)
     * @throws std::overflow_error if aₙ₊ₖ = 0 or aₙ₊ₖ₊₁ = 0 causing division by zero
     */
    T operator() (K n, K order, const series::series_base<T,K>* series, T scale = static_cast<T>(1)) const override;
};

template<FloatLike T, std::unsigned_integral K>
T v_wave_transform<T,K>::operator()(const K n, const K order, const series::series_base<T,K>* series, T scale) const  {

    using std::isfinite;

    // For theory, see: Modified v-transform with shifted indices
    // ωₙ = (aₙ₊₁ * aₙ₊₂)/(aₙ₊₁ - aₙ₊₂)
    const T a1 = series->operator()(n+order), a2 = series->operator()(n+order+1);
    const T result = (a1 - a2) / (a1 * a2);
	if (!isfinite(result)) throw std::overflow_error("division by zero");
	return result;
}