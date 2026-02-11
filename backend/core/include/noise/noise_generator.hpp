#ifndef SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP
#define SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP
#pragma once

#define pseudo_random_seed (std::chrono::system_clock::now().time_since_epoch().count() + std::rand())

#include <cstdlib>
#include <random>
#include <vector>

#include "normal_noise.hpp"
#include "poisson_noise.hpp"
#include "uniform_noise.hpp"

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
 * @brief Enum of noise application methods
 *
 * - jitter: Adds noise to partial sums.
 * - scaling: Multiplies terms by noise factor.
 */
enum NoiseMethod { jitter, scaling, noise_method_count };

/**
 * @brief Internal implementation specialized via template parameters for maximum performance.
 */
template <NoiseMethod Method, NoiseType Type, AcceptedLike T, FloatLike paramType>
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
            noise = uniform_noise<T>::generate(utils::cast<double, paramType>()(tParam1), utils::cast<double, paramType>()(tParam2), rng);
        } else if constexpr (Type == NoiseType::normal) {
            noise = normal_noise<T>::generate(utils::cast<double, paramType>()(tParam1), utils::cast<double, paramType>()(tParam2), rng);
        } else if constexpr (Type == NoiseType::poisson) {
            noise = poisson_noise<T>::generate(utils::cast<double, paramType>()(tParam1), rng);
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
template <AcceptedLike T, FloatLike paramType>
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