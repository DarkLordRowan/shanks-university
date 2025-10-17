// cpp
//
// Created by user on 03.10.2025.
//
#pragma once

#include "../custom_concepts.hpp"
#include "../utils.hpp"

#include <chrono>
#include <functional>
#include <random>
#include <vector>


#ifndef SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP
#define SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP

#define pseudo_random_seed (std::chrono::system_clock::now().time_since_epoch().count() + std::rand());


/**
* @brief Enum of noise types
*
* This enum defines different types of noise distributions that can be generated.
*
* - uniform: Represents uniform distribution noise.
* - normal: Represents normal (Gaussian) distribution noise.
* - poisson: Represents Poisson distribution noise.
*/
enum NoiseType {
    uniform,
    normal,
    poisson,
};


/**
* @brief Generates uniform noise for given type and range.
*
* This function generates uniform noise of complex-like type within the specified float-like range [inf, sup).
* Uniform noise is evenly distributed across the specified range.
* Real and imaginary parts are generated independently within the given range.
*
* @tparam CT Complex-like type for noise generation.
* @tparam FT Float-like type for range specification.

* @param inf Lower bound of the uniform distribution.
* @param sup Upper bound of the uniform distribution.\n
* Valid values: inf < sup.
* @param rng Random number generator.\n
* Valid values: any initialized std::mt19937_64 instance.
*
* @return Generated noise of type CT.
*/
template<ComplexLike CT, FloatLike FT>
requires (!ComplexLike<FT>)
CT generate_uniform_noise(const FT& inf, const FT& sup, std::mt19937_64& rng) {
    if (inf >= sup) {
        throw std::invalid_argument("Invalid borders for uniform noise generation.");
    }

    std::uniform_real_distribution<float_t> distribution(inf, sup);

    return CT(distribution(rng), distribution(rng));
};

/**
* @brief Generates uniform noise for given type and range.
*
* This function generates uniform noise of complex-like type within the specified complex-like type range [inf, sup).
* Uniform noise is evenly distributed across the specified range.
* Real and imaginary parts are generated independently within the real and imaginary parts of the given range.
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
*/
template<ComplexLike T>
T generate_uniform_noise(const T& inf, const T& sup, std::mt19937_64& rng) {
    if (inf.real() >= sup.real() || inf.imag() >= sup.imag()) {
        throw std::invalid_argument("Invalid borders for uniform noise generation.");
    }

    // Generating real and imaginary parts independently
    std::uniform_real_distribution<float_t> distribution_real(static_cast<float_t>(inf.real()), static_cast<float_t>(sup.real()));
    std::uniform_real_distribution<float_t> distribution_imag(static_cast<float_t>(inf.imag()), static_cast<float_t>(sup.imag()));

    return T(static_cast<typename T::value_type>(distribution_real(rng)), static_cast<typename T::value_type>(distribution_imag(rng)));
};

/**
* @brief Generates uniform noise for given type and range.
*
* This function generates uniform noise of float-like type within the specified float-like type range [inf, sup).
* Uniform noise is evenly distributed across the specified range.
* Noise is generated within the given range.
*
* @tparam T Float-like type for noise generation.

* @param inf Lower bound of the uniform distribution.
* @param sup Upper bound of the uniform distribution.\n
* Valid values: inf < sup.
* @param rng Random number generator.\n
* Valid values: any initialized std::mt19937_64 instance.
*
* @return Generated noise of type T.
*/
template<FloatLike T>
requires (!ComplexLike<T>)
T generate_uniform_noise(const T& inf, const T& sup, std::mt19937_64& rng) {
    if (inf >= sup) {
        throw std::invalid_argument("Invalid borders for uniform noise generation.");
    }

    std::uniform_real_distribution<float_t> distribution(static_cast<float_t>(inf), static_cast<float_t>(sup));

    return static_cast<T>(distribution(rng));
};

/**
* @brief Generates normal (Gaussian) noise for given type and range.
*
* This function generates normal (Gaussian) noise of complex-like type for the specified float-like type params.
* Normal (Gaussian) noise is distributed according to a normal distribution.
* Noise is generated independently for real and imaginary parts using the specified mean and standard deviation.
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
*/
template<ComplexLike CT, FloatLike FT>
requires (!ComplexLike<FT>)
CT generate_normal_noise(const FT& mean, const FT& std, std::mt19937_64& rng) {
    if (std <= 0) {
        throw std::invalid_argument("Standard deviation must be positive for normal distribution.");
    }

    std::normal_distribution<float_t> distribution(mean,std);

    return CT(distribution(rng), distribution(rng));
};

/**
* @brief Generates normal (Gaussian) noise for given type and range.
* This function generates normal (Gaussian) noise of complex-like type for the specified complex-like type params.
* Normal (Gaussian) noise is distributed according to a normal distribution.
* Noise is generated independently for real and imaginary parts using the specified real and imaginary parts of mean and standard deviation.
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
 */
template<ComplexLike T>
T generate_normal_noise(const T& mean, const T& std, std::mt19937_64& rng) {
    if (std.real() <= static_cast<typename T::value_type>(0) || std.imag() <= static_cast<typename T::value_type>(0)) {
        throw std::invalid_argument("Standard deviation must be positive for normal distribution.");
    }

    // Generating real and imaginary parts independently
    std::normal_distribution<float_t> distribution_real(mean.real(),std.real());
    std::normal_distribution<float_t> distribution_imag(mean.imag(),std.imag());

    return T(static_cast<typename T::value_type>(distribution_real(rng)), static_cast<typename T::value_type>(distribution_imag(rng)));
}

/** @brief Generates normal (Gaussian) noise for given type and range.
 * This function generates normal (Gaussian) noise of float-like type for the specified float-like type params.
 * Normal (Gaussian) noise is distributed according to a normal distribution.
 * Noise is generated using the specified mean and standard deviation.
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
 */
template<FloatLike T>
requires (!ComplexLike<T>)
T generate_normal_noise(const T& mean, const T& std, std::mt19937_64& rng) {
    if (std <= static_cast<T>(0)) {
        throw std::invalid_argument("Standard deviation must be positive for normal distribution.");
    }

    std::normal_distribution<float_t> distribution(static_cast<float_t>(mean), static_cast<float_t>(std));

    return static_cast<T>(distribution(rng));
};

/**
* @brief Generates Poisson noise for given type and lambda parameter.
* This function generates Poisson noise of complex-like type for the specified float-like lambda parameter which floors to integer.
* Poisson noise follows a Poisson distribution.
* Noise is generated independently for real and imaginary parts using the specified lambda parameter.
* Noise values are non-negative integers.
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
*/
template<ComplexLike CT, FloatLike FT>
requires (!ComplexLike<FT>)
CT generate_poisson_noise(const FT& lambda, std::mt19937_64& rng) {
    if (lambda <= static_cast<FT>(0)) {
        throw std::invalid_argument("Lambda must be positive for Poisson distribution.");
    }

    std::poisson_distribution<uint64_t> distribution(static_cast<uint64_t>(lambda));

    return CT(distribution(rng), distribution(rng));
};

/**
* @brief Generates Poisson noise for given type and lambda parameter.
* This function generates Poisson noise of complex-like type for the specified complex-like lambda parameter which floors to integer.
* Poisson noise follows a Poisson distribution.
* Noise is generated independently for real and imaginary parts using the specified real and imaginary parts of the lambda parameter.
* Noise values are non-negative integers.
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
*/
template<FloatLike T>
requires (!ComplexLike<T>)
T generate_poisson_noise(const T& lambda, std::mt19937_64& rng) {
    if (lambda <= static_cast<T>(0)) {
        throw std::invalid_argument("Lambda must be positive for Poisson distribution.");
    }

    std::poisson_distribution<uint64_t> distribution(static_cast<uint64_t>(lambda));

    return static_cast<T>(distribution(rng));
};

/**
* @brief Generates Poisson noise for given type and lambda parameter.
* This function generates Poisson noise of complex-like type for the specified complex-like lambda parameter which floors to integer.
* Poisson noise follows a Poisson distribution.
* Noise is generated independently for real and imaginary parts using the specified real and imaginary parts of the lambda parameter.
* Noise values are non-negative integers.
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
*/
template<ComplexLike T>
T generate_poisson_noise(const T& lambda, std::mt19937_64& rng) {
    if (lambda.real() <= static_cast<typename T::value_type>(0) || lambda.imag() <= static_cast<typename T::value_type>(0)) {
        throw std::invalid_argument("Lambda must be positive for Poisson distribution.");
    }

    // Generating real and imaginary parts independently
    std::poisson_distribution<uint64_t> distribution_real(static_cast<uint64_t>(lambda.real()));
    std::poisson_distribution<uint64_t> distribution_imag(static_cast<uint64_t>(lambda.imag()));

    return T(static_cast<typename T::value_type>(distribution_real(rng)), static_cast<typename T::value_type>(distribution_imag(rng)));
};


/** @brief Class for generating noise and applying it to series results.
* This class provides functionality to generate different types of noise
* (uniform, normal, Poisson) and apply it to series results for jittering.
* @tparam T Type of the series result elements.
*/
template<AcceptedLike T>
class noise_generator {
protected:

    unsigned long long int seed;

    std::unique_ptr<std::mt19937_64> randomNumberGen;

    NoiseType type;


    template<AcceptedLike paramType>
    inline T uniform(const paramType& inf, const paramType& sup) const {
        return generate_uniform_noise<T>(inf, sup, *randomNumberGen);
    }

    template<AcceptedLike paramType>
    inline T normal(const paramType& inf,const paramType& sup) const {
        return generate_normal_noise<T>(inf, sup, *randomNumberGen);
    };

    template<AcceptedLike paramType>
    inline T poisson(const paramType& lamda) const {
        return generate_poisson_noise<T>(lamda, *randomNumberGen);
    };


public:

    /** @brief Constructor for NoiseGenerator with specified noise type and random seed.
     *
     * This constructor initializes the NoiseGenerator with the specified noise type
     * and a random seed for generating random numbers.
     *
     * @param type The type of noise to generate (uniform, normal, poisson).
     */
    noise_generator(const NoiseType type) : type(type) {

        seed = pseudo_random_seed;

        randomNumberGen = std::make_unique<std::mt19937_64>(seed);

    }

    /** @brief Constructor for NoiseGenerator with specified noise type and defined seed.
     *
     * This constructor initializes the NoiseGenerator with the specified noise type
     * and a user-defined seed for generating random numbers.
     *
     * @param type The type of noise to generate (uniform, normal, poisson).
     * @param seed The seed for the random number generator.\n
     * Valid values: any positive integer.
     */
    noise_generator(const NoiseType type, const unsigned long long int seed) : seed(seed), type(type) {

        randomNumberGen = std::make_unique<std::mt19937_64>(seed);

    }

    /** @brief Applies jitter noise to a series result.
     *
     * This method applies jitter noise to the provided series result based on the specified noise type
     * and parameters. The noise is added to each term of the series result.
     *
     * @tparam paramType Type of the parameters for noise generation.
     *
     * @param result The series result to which noise will be applied.
     * @param tParam1 First parameter for noise generation (e.g., lower bound for uniform, mean for normal, lambda for poisson).
     * @param tParam2 Second parameter for noise generation (e.g., upper bound for uniform, std deviation for normal). Not used for poisson.
     *
     * @return A new series_result with jitter noise applied.
     */
    template<AcceptedLike paramType>
    series_result<T> jitter(const series_result<T>& result, const paramType& tParam1, const paramType& tParam2 = paramType{}) {

        std::vector<T> newSn;
        std::vector<T> newAn;

        std::function<T(const paramType&, const paramType&)> noiseFunc;

        switch (type) {
            case NoiseType::uniform:
                noiseFunc = [this](const paramType& a, const paramType& b) { return this->uniform(a, b); };
                break;
            case NoiseType::normal:
                noiseFunc = [this](const paramType& a, const paramType& b) { return this->normal(a, b); };
                break;
            case NoiseType::poisson:
                noiseFunc = [this](const paramType& a, const paramType&) { return this->poisson(a); };
                break;
        }

        for (size_t i = 0; i < result.Sn.size(); ++i) {
            T noise = noiseFunc(tParam1, tParam2);

            if (i == 0) {
                newSn.push_back(result.Sn[0] + noise);
                newAn.push_back(newSn[0]);
            } else {
                newSn.push_back(result.Sn[i] + noise);
                newAn.push_back(newSn[i] - newSn[i - 1]);
            }
        }

        return series_result<T>{.Sn = newSn, .an = newAn};
    };

    /** @brief Applies scaling noise to a series result.
     *
     * This method applies scaling noise to the provided series result based on the specified noise type
     * and parameters. The terms of the series result are scaled by the generated noise.
     *
     * @tparam paramType Type of the parameters for noise generation.
     *
     * @param result The series result to which noise will be applied.
     * @param tParam1 First parameter for noise generation (e.g., lower bound for uniform, mean for normal, lambda for poisson).
     * @param tParam2 Second parameter for noise generation (e.g., upper bound for uniform, std deviation for normal). Not used for poisson.
     *
     * @return A new series_result with scaling noise applied.
     */
    template<AcceptedLike paramType>
    series_result<T> scaling(const series_result<T>& result, const paramType& tParam1, const paramType& tParam2 = paramType{}) {
        std::vector<T> newSn;
        std::vector<T> newAn;

        std::function<T(const paramType&, const paramType&)> noiseFunc;

        switch (type) {
            case NoiseType::uniform:
                noiseFunc = [this](const paramType& a, const paramType& b) { return this->uniform(a, b); };
                break;
            case NoiseType::normal:
                noiseFunc = [this](const paramType& a, const paramType& b) { return this->normal(a, b); };
                break;
            case NoiseType::poisson:
                noiseFunc = [this](const paramType& a, const paramType&) { return this->poisson(a); };
                break;

        }
        for (size_t i = 0; i < result.Sn.size(); ++i) {
            T noise = noiseFunc(tParam1, tParam2);

            if (i == 0) {
                newSn.push_back(result.Sn[0] * noise);
                newAn.push_back(newSn[0]);
            } else {
                newAn.push_back(result.an[i] * noise);
                newSn.push_back(newSn[i - 1] + newAn[i]);
            }
        }
        return series_result<T>{.Sn = newSn, .an = newAn};
    }
};

#endif //SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP