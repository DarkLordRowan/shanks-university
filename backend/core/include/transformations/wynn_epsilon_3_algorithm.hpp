#ifndef WYNN_EPSILON_3_ALGORITHM_HPP
#define WYNN_EPSILON_3_ALGORITHM_HPP
#pragma once
/**
 * @file wynn_epsilon_3_algorithm.hpp
 * @brief This file contains the declaration of the third implementation of Wynn's Epsilon Algorithm.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G., Sobolev Y. A.,
 *
 * For theory, see:
 * Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 *   Mathematical Tables and Other Aids to Computation, 10(54), 91-96.
 * Wynn, P. (1962). A comparison technique for the numerical transformation of slowly convergent series based on the use
 * of rational functions. Numerische Mathematik, 4(1), 8-14. Wynn, P. (1964). General Purpose Vector Epsilon Algorithm
 * ALGOL Procedures. Numerische Mathematik, 6(1), 22-36.
 */

namespace shanks {
namespace algos {

/**
 * @brief Wynn's Epsilon Algorithm (Third Implementation) class template.
 *
 * This implementation combines numerical stability enhancements with efficient memory usage.
 * It computes accelerated partial sums using the epsilon algorithm with threshold-based
 * error control and adaptive table management.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements and computations (must satisfy AcceptedLike).
 *           Represents numerical precision (float, double, long double, or arbitrary precision).
 * @tparam K Unsigned integral type for indices, counts, and sizes (must satisfy UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class wynn_epsilon_3_algorithm final : public series_acceleration<T, K> {
public:
    using float_type = real_of<T>::value;  // type in case of complex or interval

    /**
     * @brief Parameterized constructor for Wynn's Epsilon Algorithm (variant 3).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param epsilon_threshold_ Threshold for epsilon corrections to maintain numerical stability.
     *        Valid values: positive T values. Too small may cause overflow, too large may reduce acceleration.
     *        Default is 1e-4 to match the original Fortran implementation (EPSALG).
     */
    explicit wynn_epsilon_3_algorithm(const float_type& epsilon_threshold_ = utils::cast<float_type, double>()(1e-4))
        : series_acceleration<T, K>("wynn epsilon 3"), epsilon_threshold(epsilon_threshold_) {};

    /**
     * @brief Executes the third implementation of the Wynn Epsilon Algorithm.
     *
     * This method applies the recursive epsilon algorithm with additional error monitoring
     * and a compacted table structure. It iteratively refines the estimate of the series limit.
     *
     * Implements the recursive epsilon algorithm with error control and adaptive table management.
     * The algorithm constructs a table of approximations and applies the recurrence:
     * For theory, see: Wynn (1956), Eq. (4): εₛ₊₁⁽ⁿ⁾ = εₛ₋₁⁽ⁿ⁺¹⁾ + 1/(εₛ⁽ⁿ⁺¹⁾ - εₛ⁽ⁿ⁾)
     * More information([https://calgo.acm.org/])
     * @authors Sobolev Y. A., Lykov D.S., Kreynin R.G.
     * @param n The starting index of the partial sum window to use.
     *        Valid values: n >= 0.
     * @param order The order of transformation (related to the depth of the table).
     *        Valid values: order >= 0.
     * @param data series_result<T> struct containing necessary information for algorithm
     * @return T The accelerated partial sum result.
     * @throws std::out_of_range if the Sn vector size is insufficient for the requested parameters.
     * @throws std::domain_error if n is 0.
     * @throws std::overflow_error if numerical instability or non-finite result occurs.
     */
    T operator()(const K n, const K order, const series_result<T>& data) const override;

private:
    const float_type
        epsilon_threshold;  ///< Threshold for epsilon correction terms to prevent division by near-zero values.
                            ///< Default: 1e-4. Smaller values may increase sensitivity but risk instability.
};

// Algorithm implementation
template <AcceptedLike T, UnsignedIntLike K>
T wynn_epsilon_3_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
    // Ensure Sn vector has enough terms: n + 2*order + 1
    // The epsilon algorithm typically requires 2*order additional terms beyond the starting point 'n'
    // to compute the diagonal approximation of that order.
    const K required_size = n + static_cast<K>(2) * order + static_cast<K>(1);
    const size_t precision = utils::helpers<T>::get_precision(data.Sn[0]);

    if (data.Sn.size() < required_size) {
        throw std::out_of_range("The Sn smaller then required for wynn_epsilon_3_{" +
                                utils::helpers<K>::to_string(order) + "}^{" + utils::helpers<K>::to_string(n) + "}\n" +
                                "the size of Sn must be at least " + utils::helpers<size_t>::to_string(required_size));
    }

    if (n == static_cast<K>(0)) throw std::domain_error("n = 0 in the input");
    if (order == static_cast<K>(0)) return data.Sn.at(n);

    T result;                  ///< Current best accelerated estimate.
    float_type abs_error;      ///< Absolute error estimate for current data.
    T resla;                   ///< Previous result for error comparison.
    K newelm, K1, ib, ie, in;  ///< Loop indices and counters.
    T RES;
    T E0, E1, E2, E3;
    T DELTA1, DELTA2, DELTA3;
    float_type ERROR, ERR1, ERR2, ERR3;
    float_type TOL1, TOL2, TOL3;
    T SS;
    result = resla = RES = E0 = E1 = E2 = E3 = DELTA1 = DELTA2 = DELTA3 = SS = utils::cast<T, int>()(0, precision);
    abs_error = ERROR = ERR1 = ERR2 = ERR3 = TOL1 = TOL2 = TOL3 = utils::cast<float_type, int>()(0, precision);

    // Epsilon table. Size should be enough to hold the diagonal.
    // Fortran used 52 for LIMEXP=50. We need approx 2*order + safety.
    // The maximum index accessed is roughly 2*order + 2.
    std::vector<T> epstab(static_cast<size_t>(2) * order + 5, utils::cast<T, int>()(0, precision));

    // Machine constants for numerical stability
    const float_type EMACH = utils::helpers<float_type>::epsilon(precision);     ///< Machine epsilon
    const float_type EPRN = utils::cast<float_type, int>()(50) * EMACH;          ///< Relative error tolerance
    const float_type OFRN = utils::helpers<float_type>::numeric_max(precision);  ///< Overflow threshold

    // Iterate through the sequence of partial sums.
    // Conceptually, we are feeding S_n, S_{n+1}, ..., S_{n + 2*order} into the algorithm.
    // 'k' here acts as the 'N' parameter in the original Fortran subroutine (if N started at 0).
    // We map 'k' to proper indices.
    const K steps = static_cast<K>(2) * order;

    // Initialize ABSERR to OFRN as in original (done implicitly via OFRN usage logic or explicit set)
    abs_error = OFRN;

    for (K k = 0; k <= steps; ++k) {
        // Load new element into the table.
        // In Fortran: EPSTAB(N) = NEW_ELEMENT (where N is 1-based index 1..steps)
        // Here we use 0-based indexing for 'epstab'.
        // Let's use 'k' as the index into 'epstab'.
        epstab[k] = data.Sn[n + k];

        // START OF EPSALG BODY Logic adaptation
        // -------------------------------------
        // EPSTAB(N+2) = EPSTAB(N) -> epstab[k+2] = epstab[k]
        // Note: Check bounds. epstab size is > k+2 since loop goes to 2*order.
        epstab[k + static_cast<K>(2)] = epstab[k];

        // NEWELM = (N-1)/2.
        // Using our 0-based k (which corresponds to Fortran N-1 if we started at N=1),
        // the formula simply becomes k/2.
        // Example: k=0 (N=1) -> newelm=0. k=1 (N=2) -> newelm=0. k=2 (N=3) -> newelm=1.
        newelm = k / static_cast<K>(2);

        // EPSTAB(N) = OFRN
        epstab[k] = OFRN;

        // ABSERR = OFRN (Resetting abs_error at each step? The Fortran code does "ABSERR = OFRN" at start of
        // subroutine) Since we are simulating sequential calls, we should likely NOT reset it if we want to track the
        // *best* result found so far? However, the Fortran routine outputs RESULT and ABSERR. If we treat this loop as
        // "finding the best result within the window", we should maintain 'result' and 'abs_error' across iterations.
        // BUT, the Fortran code: "ABSERR = OFRN" is unconditional at the start.
        // This suggests it computes the error estimate *fresh* for the current diagonal.
        // Let's stick to the Fortran logic: reset it for the current diagonal calculation.
        // We will store the *final* valid result found in this step into 'result'.
        float_type current_step_error = OFRN;
        T current_step_result = result;  // Keep previous best if this step fails? Or reset?
        // Actually, 'result' is updated only if error improves.
        // Let's follow the code flow:

        current_step_error = OFRN;

        K1 = k;
        K num_k = k;  // Equivalent to NUM = N

        bool shift_logic = true;
        bool store_best = true;

        // Loop I=1 to NEWELM
        for (K I = 1; I <= newelm; ++I) {
            K K2 = K1 - static_cast<K>(1);
            K K3 = K1 - static_cast<K>(2);

            RES = epstab[K1 + static_cast<K>(2)];
            E0 = epstab[K3];
            E1 = epstab[K2];
            E2 = RES;

            DELTA2 = E2 - E1;
            ERR2 = utils::math<T>::abs(DELTA2);
            TOL2 = std::max(utils::math<T>::abs(E2), utils::math<T>::abs(E1)) * EMACH;

            DELTA3 = E1 - E0;
            ERR3 = utils::math<T>::abs(DELTA3);
            TOL3 = std::max(utils::math<T>::abs(E1), utils::math<T>::abs(E0)) * EMACH;

            bool jump_to_10 = false;
            if (ERR2 > TOL2 || ERR3 > TOL3) {
                jump_to_10 = true;
            } else {
                // Convergence assumed
                current_step_result = RES;
                current_step_error = ERR2 + ERR3;
                // GO TO 90 (End of this step)
                shift_logic = false;
                break;
            }

            if (jump_to_10) {
                // Label 10
                E3 = epstab[K1];
                epstab[K1] = E1;

                DELTA1 = E1 - E3;
                ERR1 = utils::math<T>::abs(DELTA1);
                TOL1 = std::max(utils::math<T>::abs(E1), utils::math<T>::abs(E3)) * EMACH;

                // IF (...) GO TO 20
                if (ERR1 <= TOL1 || ERR2 <= TOL2 || ERR3 <= TOL3) {
                    // Label 20 logic inline
                    // N = I + I - 1; -> In our vars: update 'num_k' (NUM/N) to reduce table?
                    // Fortran: N becomes 2*I - 1.
                    // This affects the compaction logic later.
                    num_k = static_cast<K>(2) * I - static_cast<K>(1);
                    store_best = false;
                    break;
                }

                SS = utils::cast<T, int>()(1, precision) / DELTA1 + utils::cast<T, int>()(1, precision) / DELTA2 -
                     utils::cast<T, int>()(1, precision) / DELTA3;

                // Check for irregular behavior
                if (utils::math<T>::abs(SS * E1) <= epsilon_threshold) {  // Logic inverted from GT check
                    // Label 20 again
                    num_k = static_cast<K>(2) * I - static_cast<K>(1);
                    store_best = false;
                    break;
                }

                // Label 30
                RES = E1 + utils::cast<T, int>()(1, precision) / SS;
                epstab[K1] = RES;
                K1 -= static_cast<K>(2);

                ERROR = ERR2 + utils::math<T>::abs(RES - E2) + ERR3;
                if (ERROR <= current_step_error) {
                    current_step_error = ERROR;
                    current_step_result = RES;
                }
                // Label 40 CONTINUE
            }
        }

        // Compaction / Table Shift Logic
        // Fortran: IF (N.EQ.LIMEXP) N = 2*(LIMEXP/2) - 1 ...
        // We don't have fixed LIMEXP, but we can perform the compaction to keep 'epstab' clean
        // or just let it grow. The algorithm assumes compaction to remove old diagonals.
        // Implementing compaction (Shift the table)
        // IB = 1. IF ((NUM/2)*2.EQ.NUM) IB = 2.
        // In 0-based: if k (NUM) is odd -> IB=0?
        // Fortran NUM=1 (Odd) -> IB=1. C++ k=0 (Even) -> ib=0.
        // Fortran NUM=2 (Even) -> IB=2. C++ k=1 (Odd) -> ib=1.
        if (shift_logic) {
            ib = (k % static_cast<K>(2) == static_cast<K>(0)) ? static_cast<K>(0) : static_cast<K>(1);

            ie = newelm + static_cast<K>(1);

            for (K i_comp = 0; i_comp < ie; ++i_comp) {
                K ib2 = ib + static_cast<K>(2);
                // EPSTAB(IB) = EPSTAB(IB2)
                epstab[ib] = epstab[ib2];
                ib = ib2;
            }

            if (k != num_k) {  // IF (NUM.EQ.N) check (k is original N, num_k is potentially modified N)
                // Fortran: IN = NUM - N + 1.
                // 0-based: index 'in' = k - num_k.
                in = k - num_k;
                for (K j = 0; j <= num_k; ++j) {  // <= num_k to cover 0..N?
                    // Fortran loops 1 to N.
                    // So we loop 0 to num_k? Wait, if N=num_k (0-based count?), loop 0 to num_k?
                    // If num_k is index, loop j=0 to num_k?
                    // Yes.
                    // epstab[j] = epstab[in + j]
                    if (in + j < epstab.size()) epstab[j] = epstab[in + j];
                }
            }
        }

        // Label 80
        // ABSERR = ABS(RESULT - RESLA)
        // RESLA = RESULT

        // Label 90
        // ABSERR = MAX(...)
        // Store best result for this step.
        // The algorithm returns the result of the *last* step (or the most converged one).
        // We update the class 'result' variable.
        if (store_best) {
            result = current_step_result;

            // Error update for next step comparison?
            abs_error = std::max(utils::math<T>::abs(result - resla), EPRN * utils::math<T>::abs(result));
            resla = result;
        }
    }

    // Final validity check for the accelerated estimate
    if (!utils::helpers<T>::isfinite(result)) throw std::overflow_error("division by zero");

    return result;
}

}  // namespace algos
}  // namespace shanks

#endif