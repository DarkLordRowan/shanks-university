#pragma once

#include <chrono>
#include <cstdlib>
#include <functional>
#include <random>
#include <vector>

#include "../custom_concepts.hpp"
#include "../utils.hpp"

#ifndef SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP
#define SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP

#define pseudo_random_seed (std::chrono::system_clock::now().time_since_epoch().count() + std::rand())

/**
 * @file noise_generator.hpp
 * @brief This file contains the generator of noise.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

/**
 * @brief Enum of noise types
 *
 * This enum defines different types of noise distributions that can be generated.
 *
 * - uniform: Represents uniform distribution noise.
 * - normal: Represents normal (Gaussian) distribution noise.
 * - poisson: Represents Poisson distribution noise.
 */
enum NoiseType { uniform = 0, normal = 1, poisson = 2, noise_count = 3 };

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
    if (inf >= sup) {
        throw std::invalid_argument("Invalid borders for uniform noise generation.");
    }

    // Creating uniform distribution for floating point values
    std::uniform_real_distribution<float_t> distribution(inf, sup);

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
    if (inf.real() >= sup.real() || inf.imag() >= sup.imag()) {
        throw std::invalid_argument("Invalid borders for uniform noise generation.");
    }

    // Generating real and imaginary parts independently using respective bounds
    std::uniform_real_distribution<float_t> distribution_real(utils::cast<float_t>(inf.real()),
                                                              utils::cast<float_t>(sup.real()));
    std::uniform_real_distribution<float_t> distribution_imag(utils::cast<float_t>(inf.imag()),
                                                              utils::cast<float_t>(sup.imag()));

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
    if (inf.leftinterval() >= sup.rightinterval()) {
        throw std::invalid_argument("Invalid borders for uniform noise generation.");
    }

    // Drawing a random value from within the specified interval range
    std::uniform_real_distribution<float_t> distribution(utils::cast<float_t>(inf.leftinterval()),
                                                         utils::cast<float_t>(sup.rightinterval()));

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
    if (inf >= sup) {
        throw std::invalid_argument("Invalid borders for uniform noise generation.");
    }

    // Simple uniform distribution generation for scalar types
    std::uniform_real_distribution<float_t> distribution(utils::cast<float_t>(inf), utils::cast<float_t>(sup));

    return utils::cast<T>(distribution(rng));
};

/**
 * @brief Generates normal (Gaussian) noise for given type and range.
 *
 * This function generates normal (Gaussian) noise of complex-like type for the specified float-like type params.
 * Normal (Gaussian) noise is distributed according to a normal distribution.
 * Noise is generated independently for real and imaginary parts using the specified mean and standard deviation.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam CT Complex-like type for noise generation.
 * @tparam FT Float-like type for distribution parameters.
 *
 * @param mean Mean of the normal (Gaussian) distribution.\n
 * Valid values: any real number.\n
 * Defines the center of the normal distribution.
 * @param std Standard deviation of the normal (Gaussian) distribution.\n
 * Valid values: std > 0.\n
 * Defines the spread of the normal distribution.
 * @param rng Random number generator.\n
 * Valid values: any initialized std::mt19937_64 instance.
 *
 * @return Generated noise of type CT.
 * @throws std::invalid_argument if std <= 0
 */
template <ComplexLike CT, FloatLike FT>
    requires(!ComplexLike<FT> && !IntervalLike<FT>)
CT generate_normal_noise(const FT& mean, const FT& std, std::mt19937_64& rng) {
    if (std <= 0) {
        throw std::invalid_argument("Standard deviation must be positive for normal distribution.");
    }

    // Creating normal distribution with specified parameters
    std::normal_distribution<float_t> distribution(mean, std);

    // Both parts share the same distribution parameters
    return CT(distribution(rng), distribution(rng));
};

/**
 * @brief Generates normal (Gaussian) noise for given type and range.
 * This function generates normal (Gaussian) noise of complex-like type for the specified complex-like type params.
 * Normal (Gaussian) noise is distributed according to a normal distribution.
 * Noise is generated independently for real and imaginary parts using the specified real and imaginary parts of mean
 * and standard deviation.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam T Complex-like type for noise generation.
 *
 * @param mean Complex mean of the normal (Gaussian) distribution.\n
 * Valid values: mean.real() and mean.imag() can be any real numbers.\n
 * Defines the center of the normal distributio for both real and imaginary parts.
 * @param std Complex standard deviation of the normal (Gaussian) distribution.\n
 * Valid values: std.real() > 0 and std.imag() > 0.\n
 * Defines the spread of the normal distribution for both real and imaginary parts.
 * @param rng Random number generator.\n
 * Valid values: any initialized std::mt19937_64 instance.
 *
 * @return Generated noise of type T.
 * @throws std::invalid_argument if any standard deviation part is non-positive
 */
template <ComplexLike T>
T generate_normal_noise(const T& mean, const T& std, std::mt19937_64& rng) {
    if (std.real() <= static_cast<typename T::value_type>(0) || std.imag() <= static_cast<typename T::value_type>(0)) {
        throw std::invalid_argument("Standard deviation must be positive for normal distribution.");
    }

    // Generating real and imaginary parts independently with their own mean/std
    std::normal_distribution<float_t> distribution_real(utils::cast<float_t>(mean.real()),
                                                        utils::cast<float_t>(std.real()));
    std::normal_distribution<float_t> distribution_imag(utils::cast<float_t>(mean.imag()),
                                                        utils::cast<float_t>(std.imag()));

    return T(static_cast<typename T::value_type>(distribution_real(rng)),
             static_cast<typename T::value_type>(distribution_imag(rng)));
}

/**
 * @brief Generates normal noise for interval types
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param mean (T), std (T), rng (std::mt19937_64)
 * @return T (Generated noise)
 * @throws std::invalid_argument if std dev is non-positive
 */
template <IntervalLike T>
T generate_normal_noise(const T& mean, const T& std, std::mt19937_64& rng) {
    if (std.leftinterval() <= static_cast<typename T::value_type>(0) ||
        std.leftinterval() <= static_cast<typename T::value_type>(0)) {
        throw std::invalid_argument("Standard deviation must be positive for normal distribution.");
    }

    // Using left interval boundary for distribution generation
    std::normal_distribution<float_t> distribution(mean.leftinterval(), std.leftinterval());

    return T(static_cast<typename T::value_type>(distribution(rng)));
}

/** @brief Generates normal (Gaussian) noise for given type and range.
 * This function generates normal (Gaussian) noise of float-like type for the specified float-like type params.
 * Normal (Gaussian) noise is distributed according to a normal distribution.
 * Noise is generated using the specified mean and standard deviation.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam T Float-like type for noise generation.
 *
 * @param mean Mean of the normal (Gaussian) distribution.\n
 * Valid values: any real number.\n
 * Defines the center of the normal distribution.
 * @param std Standard deviation of the normal (Gaussian) distribution.\n
 * Valid values: std > 0.\n
 * Defines the spread of the normal distribution.
 * @param rng Random number generator.\n
 * Valid values: any initialized std::mt19937_64 instance.
 *
 * @return Generated noise of type T.
 * @throws std::invalid_argument if std <= 0
 */
template <FloatLike T>
    requires(!ComplexLike<T> && !IntervalLike<T>)
T generate_normal_noise(const T& mean, const T& std, std::mt19937_64& rng) {
    if (std <= utils::cast<T>(0))
        throw std::invalid_argument("Standard deviation must be positive for normal distribution.");

    // Simple Gaussian distribution for scalar types
    std::normal_distribution<float_t> distribution(utils::cast<float_t>(mean), utils::cast<float_t>(std));

    return utils::cast<T>(distribution(rng));
}

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
    if (lambda <= utils::cast<FT>(0)) {
        throw std::invalid_argument("Lambda must be positive for Poisson distribution.");
    }

    // Generating discrete Poisson distribution values
    std::poisson_distribution<uint64_t> distribution(utils::cast<uint64_t>(lambda));

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
    if (lambda <= utils::cast<T>(0)) throw std::invalid_argument("Lambda must be positive for Poisson distribution.");

    // Simple Poisson distribution for scalar types
    std::poisson_distribution<uint64_t> distribution(utils::cast<uint64_t>(lambda));

    return utils::cast<T>(distribution(rng));
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
        lambda.imag() <= utils::cast<typename T::value_type>(0)) {
        throw std::invalid_argument("Lambda must be positive for Poisson distribution.");
    }

    // Generating real and imaginary parts independently with their own lambda rates
    std::poisson_distribution<uint64_t> distribution_real(utils::cast<uint64_t>(lambda.real()));
    std::poisson_distribution<uint64_t> distribution_imag(utils::cast<uint64_t>(lambda.imag()));

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
    if (lambda.leftinterval() <= utils::cast<typename T::value_type>(0)) {
        throw std::invalid_argument("Lambda must be positive for Poisson distribution.");
    }

    // Using left boundary for Poisson rate calculation
    std::poisson_distribution<uint64_t> distribution(utils::cast<uint64_t>(lambda.leftinterval()));

    return T(utils::cast<typename T::value_type>(distribution(rng)));
};

/**
 * @brief Enum of noise application methods
 *
 * - jitter: Adds noise to partial sums.
 * - scaling: Multiplies terms by noise factor.
 */
enum NoiseMethod { jitter, scaling, noise_method_count };

/**
 * @brief Internal implementation specialized via template parameters for maximum performance.
 */
template <NoiseMethod Method, NoiseType Type, AcceptedLike T, AcceptedLike paramType>
series_result<T> apply_noise_impl(const series_result<T>& result, std::mt19937_64& rng, const paramType& tParam1,
                                  const paramType& tParam2) {
    const size_t size = result.Sn.size();
    std::vector<T> newSn;
    std::vector<T> newAn;
    newSn.reserve(size);
    newAn.reserve(size);

    for (size_t i = 0; i < size; ++i) {
        T noise;
        if constexpr (Type == NoiseType::uniform) {
            noise = generate_uniform_noise<T>(tParam1, tParam2, rng);
        } else if constexpr (Type == NoiseType::normal) {
            noise = generate_normal_noise<T>(tParam1, tParam2, rng);
        } else if constexpr (Type == NoiseType::poisson) {
            noise = generate_poisson_noise<T>(tParam1, rng);
        }

        if constexpr (Method == NoiseMethod::jitter) {
            if (i == 0) {
                newSn.push_back(result.Sn[0] + noise);
                newAn.push_back(newSn[0]);
            } else {
                newSn.push_back(result.Sn[i] + noise);
                newAn.push_back(newSn[i] - newSn[i - 1]);
            }
        } else if constexpr (Method == NoiseMethod::scaling) {
            if (i == 0) {
                newSn.push_back(result.Sn[0] * noise);
                newAn.push_back(newSn[0]);
            } else {
                newAn.push_back(result.an[i] * noise);
                newSn.push_back(newSn[i - 1] + newAn[i]);
            }
        }
    }
    return series_result<T>{.Sn = std::move(newSn), .an = std::move(newAn)};
}

/**
 * @brief Applies noise to a series result using specified method and distribution.
 *
 * This function dispatches runtime enums to template-specialized implementations.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
template <AcceptedLike T, AcceptedLike paramType>
series_result<T> apply_noise(const series_result<T>& result, const NoiseMethod method, const NoiseType type,
                             const unsigned long long int seed, const paramType& tParam1,
                             const paramType& tParam2 = paramType{}) {
    std::mt19937_64 rng(seed);

    auto dispatch_type = [&](auto method_const) {
        switch (type) {
            case NoiseType::uniform:
                return apply_noise_impl<decltype(method_const)::value, NoiseType::uniform, T, paramType>(
                    result, rng, tParam1, tParam2);
            case NoiseType::normal:
                return apply_noise_impl<decltype(method_const)::value, NoiseType::normal, T, paramType>(
                    result, rng, tParam1, tParam2);
            case NoiseType::poisson:
                return apply_noise_impl<decltype(method_const)::value, NoiseType::poisson, T, paramType>(
                    result, rng, tParam1, tParam2);
            default:
                throw std::invalid_argument("Invalid noise type");
        }
    };

    switch (method) {
        case NoiseMethod::jitter:
            return dispatch_type(std::integral_constant<NoiseMethod, NoiseMethod::jitter>{});
        case NoiseMethod::scaling:
            return dispatch_type(std::integral_constant<NoiseMethod, NoiseMethod::scaling>{});
        default:
            throw std::invalid_argument("Invalid noise method");
    }
};

#endif  // SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP