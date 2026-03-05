#ifndef SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP
#define SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP
#pragma once
#include <chrono>

#include <cstdlib>
#include <random>
#include <stdexcept>
#include <vector>

#include "normal_noise.hpp"
#include "poisson_noise.hpp"
#include "uniform_noise.hpp"
#include "utils/json.hpp"

namespace shanks {

#define pseudo_random_seed (std::chrono::system_clock::now().time_since_epoch().count() + std::rand())

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
 * @brief Internal implementation for Uniform noise.
 */
template <NoiseMethod Method, NoiseType Type, AcceptedLike T>
    requires(Type == NoiseType::uniform)
series_result<T> apply_noise_impl(const series_result<T>& result, std::mt19937_64& rng, double min, double max) {
    const size_t size = result.Sn.size();
    std::vector<T> newSn;
    std::vector<T> newAn;
    newSn.reserve(size);
    newAn.reserve(size);

    for (size_t i = 0; i < size; ++i) {
        using Underlying = typename GetUnderlyingType<T>::value;
        T noise = uniform_noise<T>::generate(::utils::cast<Underlying, double>()(min),
                                             ::utils::cast<Underlying, double>()(max), rng);

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
 * @brief Internal implementation for Normal noise.
 */
template <NoiseMethod Method, NoiseType Type, AcceptedLike T>
    requires(Type == NoiseType::normal)
series_result<T> apply_noise_impl(const series_result<T>& result, std::mt19937_64& rng, double mean, double stddev) {
    const size_t size = result.Sn.size();
    std::vector<T> newSn;
    std::vector<T> newAn;
    newSn.reserve(size);
    newAn.reserve(size);

    for (size_t i = 0; i < size; ++i) {
        T noise = normal_noise<T>::generate(mean, stddev, rng);

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
 * @brief Internal implementation for Poisson noise.
 */
template <NoiseMethod Method, NoiseType Type, AcceptedLike T>
    requires(Type == NoiseType::poisson)
series_result<T> apply_noise_impl(const series_result<T>& result, std::mt19937_64& rng, double lambda) {
    const size_t size = result.Sn.size();
    std::vector<T> newSn;
    std::vector<T> newAn;
    newSn.reserve(size);
    newAn.reserve(size);

    for (size_t i = 0; i < size; ++i) {
        T noise = poisson_noise<T>::generate(lambda, rng);

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
 * @brief Applies Uniform noise to a series result.
 */
template <AcceptedLike T>
series_result<T> apply_uniform_noise(const series_result<T>& result, const NoiseMethod method,
                                     const unsigned long long seed, double min, double max) {
    std::mt19937_64 rng(seed);
    if (method == NoiseMethod::jitter)
        return apply_noise_impl<NoiseMethod::jitter, NoiseType::uniform, T>(result, rng, min, max);
    return apply_noise_impl<NoiseMethod::scaling, NoiseType::uniform, T>(result, rng, min, max);
}

/**
 * @brief Applies Normal noise to a series result.
 */
template <AcceptedLike T>
series_result<T> apply_normal_noise(const series_result<T>& result, const NoiseMethod method,
                                    const unsigned long long seed, double mean, double stddev) {
    std::mt19937_64 rng(seed);
    if (method == NoiseMethod::jitter)
        return apply_noise_impl<NoiseMethod::jitter, NoiseType::normal, T>(result, rng, mean, stddev);
    return apply_noise_impl<NoiseMethod::scaling, NoiseType::normal, T>(result, rng, mean, stddev);
}


/**
 * @brief Applies Poisson noise to a series result.
 */
template <AcceptedLike T>
series_result<T> apply_poisson_noise(const series_result<T>& result, const NoiseMethod method,
                                     const unsigned long long seed, double lambda) {
    std::mt19937_64 rng(seed);
    if (method == NoiseMethod::jitter)
        return apply_noise_impl<NoiseMethod::jitter, NoiseType::poisson, T>(result, rng, lambda);
    return apply_noise_impl<NoiseMethod::scaling, NoiseType::poisson, T>(result, rng, lambda);
}

/**
 * @brief High-level entry point for applying noise using string-based type/method and JSON parameters.
 */
template <AcceptedLike T>
series_result<T> apply_noise(const series_result<T>& result, const std::string& name,
                             const std::string& params_json, uint64_t start_n) {
    // 1. Handle tail slicing
    series_result<T> tail_res = result;
    if (start_n > 0 && start_n < result.Sn.size()) {
        tail_res.Sn.erase(tail_res.Sn.begin(), tail_res.Sn.begin() + start_n);
        if (tail_res.an.size() >= start_n) {
            tail_res.an.erase(tail_res.an.begin(), tail_res.an.begin() + start_n);
        }
    }

    // 2. Parse Method and Type
    NoiseType nt = NoiseType::uniform;
    if (name == "uniform") nt = NoiseType::uniform;
    else if (name == "normal" || name == "gaussian") nt = NoiseType::normal;
    else if (name == "poisson") nt = NoiseType::poisson;

    NoiseMethod nm = NoiseMethod::jitter;
    auto method_str = ::shanks::utils::get_json_val(params_json, "method");
    if (method_str == "scaling") nm = NoiseMethod::scaling;

    // 3. Parse Seed
    unsigned long long seed = 0;
    try {
        auto seed_str = ::shanks::utils::get_json_val(params_json, "seed");
        if (!seed_str.empty()) seed = std::stoull(seed_str);
    } catch (...) {}

    // 4. Validate and Dispatch to specific noise implementations
    if (nt == NoiseType::uniform) {
        double min = 0, max = 0;
        try { min = std::stod(::shanks::utils::get_json_val(params_json, "min")); } catch (...) {}
        try { max = std::stod(::shanks::utils::get_json_val(params_json, "max")); } catch (...) {}
        
        if (min >= max) {
            throw std::invalid_argument("uniform noise: min must be less than max (min=" + std::to_string(min) + ", max=" + std::to_string(max) + ")");
        }
        return apply_uniform_noise<T>(tail_res, nm, seed, min, max);
    } else if (nt == NoiseType::normal) {
        double mean = 0, stddev = 0;
        try { mean = std::stod(::shanks::utils::get_json_val(params_json, "mean")); } catch (...) {}
        try { stddev = std::stod(::shanks::utils::get_json_val(params_json, "stddev")); } catch (...) {}
        
        if (stddev <= 0) {
            throw std::invalid_argument("normal noise: stddev must be positive (stddev=" + std::to_string(stddev) + ")");
        }
        return apply_normal_noise<T>(tail_res, nm, seed, mean, stddev);
    } else if (nt == NoiseType::poisson) {
        double lambda = 0;
        try { lambda = std::stod(::shanks::utils::get_json_val(params_json, "lambda")); } catch (...) {}
        
        if (lambda <= 0) {
            throw std::invalid_argument("poisson noise: lambda must be positive (lambda=" + std::to_string(lambda) + ")");
        }
        return apply_poisson_noise<T>(tail_res, nm, seed, lambda);
    }
    return tail_res;
}

} // namespace shanks

#endif  // SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP