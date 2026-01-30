#ifndef UNIFORM_NOISE_HPP
#define UNIFORM_NOISE_HPP

#include <random>

#include "../custom_concepts.hpp"
#include "../utils.hpp"

/**
 * @brief Generates uniform noise for given type and range.
 *
 * This function generates uniform noise of complex-like type within the specified float-like range [inf, sup).
 * Uniform noise is evenly distributed across the specified range.
 * Real and imaginary parts are generated independently within the given range.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam CT Complex-like type for noise generation.
 * @tparam FT Float-like type for range specification.
 *
 * @param inf Lower bound of the uniform distribution.
 * @param sup Upper bound of the uniform distribution.\n
 * Valid values: inf < sup.
 * @param rng Random number generator.\n
 * Valid values: any initialized std::mt19937_64 instance.
 *
 * @return Generated noise of type CT.
 * @throws std::invalid_argument if inf >= sup
 */
template <ComplexLike CT, FloatLike FT>
    requires(!ComplexLike<FT>)
CT generate_uniform_noise(const FT& inf, const FT& sup, std::mt19937_64& rng) {
    if (inf >= sup) throw std::invalid_argument("Invalid borders for uniform noise generation.");

    // Creating uniform distribution for floating point values
    std::uniform_real_distribution<float_t> distribution{inf, sup};

    // Returning complex noise with independent parts
    return CT(distribution(rng), distribution(rng));
};

/**
 * @brief Generates uniform noise for given type and range.
 *
 * This function generates uniform noise of complex-like type within the specified complex-like type range [inf, sup).
 * Uniform noise is evenly distributed across the specified range.
 * Real and imaginary parts are generated independently within the real and imaginary parts of the given range.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam T Complex-like type for noise generation.
 *
 * @param inf Complex lower bound of the uniform distribution.
 * @param sup Complex upper bound of the uniform distribution.\n
 * Valid values: inf.real() < sup.real() and inf.imag() < sup.imag().
 * @param rng Random number generator.\n
 * Valid values: any initialized std::mt19937_64 instance.
 *
 * @return Generated noise of type T.
 * @throws std::invalid_argument if bounds are inconsistent
 */
template <ComplexLike T>
T generate_uniform_noise(const T& inf, const T& sup, std::mt19937_64& rng) {
    if (inf.real() >= sup.real() || inf.imag() >= sup.imag())
        throw std::invalid_argument("Invalid borders for uniform noise generation.");

    // Generating real and imaginary parts independently using respective bounds
    std::uniform_real_distribution<float_t> distribution_real{
        utils::cast<float_t, typename real_of<T>::value>()(inf.real()),
        utils::cast<float_t, typename real_of<T>::value>()(sup.real())};
    std::uniform_real_distribution<float_t> distribution_imag{
        utils::cast<float_t, typename real_of<T>::value>()(inf.imag()),
        utils::cast<float_t, typename real_of<T>::value>()(sup.imag())};

    return T(static_cast<typename T::value_type>(distribution_real(rng)),
             static_cast<typename T::value_type>(distribution_imag(rng)));
};

/**
 * @brief Generates uniform noise for interval types
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Complex-like type for noise generation.
 * @param inf (T), sup (T), rng (std::mt19937_64)
 * @return T (Generated noise)
 * @throws std::invalid_argument if left border >= right border
 */
template <IntervalLike T>
T generate_uniform_noise(const T& inf, const T& sup, std::mt19937_64& rng) {
    if (inf.leftinterval() >= sup.rightinterval())
        throw std::invalid_argument("Invalid borders for uniform noise generation.");

    // Drawing a random value from within the specified interval range
    std::uniform_real_distribution<float_t> distribution{
        utils::cast<float_t, typename T::value_type>()(inf.leftinterval()),
        utils::cast<float_t, typename T::value_type>()(sup.rightinterval())};

    return T(static_cast<typename T::value_type>(distribution(rng)));
};

/**
 * @brief Generates uniform noise for given type and range.
 *
 * This function generates uniform noise of float-like type within the specified float-like type range [inf, sup).
 * Uniform noise is evenly distributed across the specified range.
 * Noise is generated within the given range.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam T Float-like type for noise generation.
 *
 * @param inf Lower bound of the uniform distribution.
 * @param sup Upper bound of the uniform distribution.\n
 * Valid values: inf < sup.
 * @param rng Random number generator.\n
 * Valid values: any initialized std::mt19937_64 instance.
 *
 * @return Generated noise of type T.
 * @throws std::invalid_argument if inf >= sup
 */
template <FloatLike T>
    requires(!ComplexLike<T>)
T generate_uniform_noise(const T& inf, const T& sup, std::mt19937_64& rng) {
    if (inf >= sup) throw std::invalid_argument("Invalid borders for uniform noise generation.");

    // Simple uniform distribution generation for scalar types
    std::uniform_real_distribution<float_t> distribution{utils::cast<float_t, T>()(inf),
                                                         utils::cast<float_t, T>()(sup)};

    return utils::cast<T, float_t>()(distribution(rng));
};

#endif