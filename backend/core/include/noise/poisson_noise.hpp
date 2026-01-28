#ifndef POISSSON_NOISE_HPP
#define POISSSON_NOISE_HPP

#include <random>

#include "../custom_concepts.hpp"
#include "../utils.hpp"

/**
 * @brief Generates Poisson noise for given type and lambda parameter.
 * This function generates Poisson noise of complex-like type for the specified float-like lambda parameter which floors
 * to integer. Poisson noise follows a Poisson distribution. Noise is generated independently for real and imaginary
 * parts using the specified lambda parameter. Noise values are non-negative integers.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam CT Complex-like type for noise generation.
 * @tparam FT Float-like type for lambda parameter.
 *
 * @param lambda Lambda parameter of the Poisson distribution.\n
 * Valid values: lambda > 0.\n
 * Defines the average rate (mean) of occurrence for the Poisson distribution.
 * @param rng Random number generator.\n
 * Valid values: any initialized std::mt19937_64 instance.
 *
 * @return Generated noise of type CT.
 * @throws std::invalid_argument if lambda <= 0
 */
template <ComplexLike CT, FloatLike FT>
    requires(!ComplexLike<FT>)
CT generate_poisson_noise(const FT& lambda, std::mt19937_64& rng) {
    if (lambda <= utils::cast<FT>(0)) throw std::invalid_argument("Lambda must be positive for Poisson distribution.");

    // Generating discrete Poisson distribution values
    std::poisson_distribution<uint64_t> distribution{utils::cast<uint64_t>(lambda)};

    return CT(distribution(rng), distribution(rng));
};

/**
 * @brief Generates Poisson noise for given type and lambda parameter.
 * This function generates Poisson noise of float-like type for the specified float-like lambda parameter which floors
 * to integer. Poisson noise follows a Poisson distribution. Noise is generated using the specified lambda parameter.
 * Noise values are non-negative integers.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam T Float-like type for noise generation.
 *
 * @param lambda Lambda parameter of the Poisson distribution.\n
 * Valid values: lambda > 0.\n
 * Defines the average rate (mean) of occurrence for the Poisson distribution.
 * @param rng Random number generator.\n
 * Valid values: any initialized std::mt19937_64 instance.
 *
 * @return Generated noise of type T.
 * @throws std::invalid_argument if lambda <= 0
 */
template <FloatLike T>
    requires(!ComplexLike<T>)
T generate_poisson_noise(const T& lambda, std::mt19937_64& rng) {
    if (lambda <= utils::cast<T>::meta(0)) throw std::invalid_argument("Lambda must be positive for Poisson distribution.");

    // Simple Poisson distribution for scalar types
    std::poisson_distribution<uint64_t> distribution{utils::cast<uint64_t>(lambda)};

    return utils::cast<T>::meta(distribution(rng));
};

/**
 * @brief Generates Poisson noise for given type and lambda parameter.
 * This function generates Poisson noise of complex-like type for the specified complex-like lambda parameter which
 * floors to integer. Poisson noise follows a Poisson distribution. Noise is generated independently for real and
 * imaginary parts using the specified real and imaginary parts of the lambda parameter. Noise values are non-negative
 * integers.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam T Complex-like type for noise generation.
 *
 * @param lambda Complex lambda parameter of the Poisson distribution.\n
 * Valid values: lambda.real() > 0 and lambda.imag() > 0.\n
 * Defines the average rate (mean) of occurrence for the Poisson distribution for both real and imaginary parts.
 * @param rng Random number generator.\n
 * Valid values: any initialized std::mt19937_64 instance.
 *
 * @return Generated noise of type T.
 * @throws std::invalid_argument if lambda parts are non-positive
 */
template <ComplexLike T>
T generate_poisson_noise(const T& lambda, std::mt19937_64& rng) {
    if (lambda.real() <= utils::cast<typename T::value_type>(0) ||
        lambda.imag() <= utils::cast<typename T::value_type>(0))
        throw std::invalid_argument("Lambda must be positive for Poisson distribution.");

    // Generating real and imaginary parts independently with their own lambda rates
    std::poisson_distribution<uint64_t> distribution_real{utils::cast<uint64_t>(lambda.real())};
    std::poisson_distribution<uint64_t> distribution_imag{utils::cast<uint64_t>(lambda.imag())};

    return T(utils::cast<typename T::value_type>(distribution_real(rng)),
             utils::cast<typename T::value_type>(distribution_imag(rng)));
};

/**
 * @brief Generates Poisson noise for interval types
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param lambda (T), rng (std::mt19937_64)
 * @return T (Generated noise)
 * @throws std::invalid_argument if lambda border is non-positive
 */
template <IntervalLike T>
T generate_poisson_noise(const T& lambda, std::mt19937_64& rng) {
    if (lambda.leftinterval() <= utils::cast<typename T::value_type>(0))
        throw std::invalid_argument("Lambda must be positive for Poisson distribution.");

    // Using left boundary for Poisson rate calculation
    std::poisson_distribution<uint64_t> distribution{utils::cast<uint64_t>(lambda.leftinterval())};

    return T(utils::cast<typename T::value_type>(distribution(rng)));
};

#endif