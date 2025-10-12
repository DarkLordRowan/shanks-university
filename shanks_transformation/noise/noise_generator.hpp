// cpp
//
// Created by user on 03.10.2025.
//
#pragma once

#include "../custom_concepts.hpp"

#include <chrono>
#include <random>
#include <vector>

#ifndef SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP
#define SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP

#define pseudo_random_seed (std::chrono::system_clock::now().time_since_epoch().count() + std::rand());

enum NoiseType {
    uniform,
    normal,
    poisson,
};

template<ComplexLike CT, FloatLike FT>
requires (!ComplexLike<FT>)
CT generate_uniform_noise(const FT& inf, const FT& sup, std::mt19937_64& rng) {
    if (inf >= sup) {
        throw std::invalid_argument("Invalid borders for uniform noise generation.");
    }

    std::uniform_real_distribution<float_t> distribution(inf, sup);

    return CT(distribution(rng), distribution(rng));
};

template<ComplexLike T>
T generate_uniform_noise(const T& inf, const T& sup, std::mt19937_64& rng) {
    if (inf.real() >= sup.real() || inf.imag() >= sup.imag()) {
        throw std::invalid_argument("Invalid borders for uniform noise generation.");
    }

    std::uniform_real_distribution<float_t> distribution_real(static_cast<float_t>(inf.real()), static_cast<float_t>(sup.real()));
    std::uniform_real_distribution<float_t> distribution_imag(static_cast<float_t>(inf.imag()), static_cast<float_t>(sup.imag()));

    return T(static_cast<typename T::value_type>(distribution_real(rng)), static_cast<typename T::value_type>(distribution_imag(rng)));
};

template<FloatLike T>
requires (!ComplexLike<T>)
T generate_uniform_noise(const T& inf, const T& sup, std::mt19937_64& rng) {
    if (inf >= sup) {
        throw std::invalid_argument("Invalid borders for uniform noise generation.");
    }

    std::uniform_real_distribution<float_t> distribution(static_cast<float_t>(inf), static_cast<float_t>(sup));

    return static_cast<T>(distribution(rng));
};

template<ComplexLike CT, FloatLike FT>
requires (!ComplexLike<FT>)
CT generate_normal_noise(const FT& mean, const FT& std, std::mt19937_64& rng) {
    if (std <= 0) {
        throw std::invalid_argument("Standard deviation must be positive for normal distribution.");
    }

    std::normal_distribution<float_t> distribution(mean,std);

    return CT(distribution(rng), distribution(rng));
};

template<ComplexLike T>
T generate_normal_noise(const T& mean, const T& std, std::mt19937_64& rng) {
    if (std.real() <= static_cast<typename T::value_type>(0) || std.imag() <= static_cast<typename T::value_type>(0)) {
        throw std::invalid_argument("Standard deviation must be positive for normal distribution.");
    }

    std::normal_distribution<float_t> distribution_real(mean.real(),std.real());
    std::normal_distribution<float_t> distribution_imag(mean.imag(),std.imag());

    return T(static_cast<typename T::value_type>(distribution_real(rng)), static_cast<typename T::value_type>(distribution_imag(rng)));
}

template<FloatLike T>
requires (!ComplexLike<T>)
T generate_normal_noise(const T& mean, const T& std, std::mt19937_64& rng) {
    if (std <= static_cast<T>(0)) {
        throw std::invalid_argument("Standard deviation must be positive for normal distribution.");
    }

    std::normal_distribution<float_t> distribution(static_cast<float_t>(mean), static_cast<float_t>(std));

    return static_cast<T>(distribution(rng));
};

//std::poisson_distribution - only for int
template<ComplexLike CT, FloatLike FT>
requires (!ComplexLike<FT>)
CT generate_poisson_noise(const FT& lambda, std::mt19937_64& rng) {
    if (lambda <= static_cast<FT>(0)) {
        throw std::invalid_argument("Lambda must be positive for Poisson distribution.");
    }

    std::poisson_distribution<uint64_t> distribution(static_cast<uint64_t>(lambda));

    return CT(distribution(rng), distribution(rng));
};

template<FloatLike T>
requires (!ComplexLike<T>)
T generate_poisson_noise(const T& lambda, std::mt19937_64& rng) {
    if (lambda <= static_cast<T>(0)) {
        throw std::invalid_argument("Lambda must be positive for Poisson distribution.");
    }

    std::poisson_distribution<uint64_t> distribution(static_cast<uint64_t>(lambda));

    return static_cast<T>(distribution(rng));
};

template<ComplexLike T>
T generate_poisson_noise(const T& lambda, std::mt19937_64& rng) {
    if (lambda.real() <= static_cast<typename T::value_type>(0) || lambda.imag() <= static_cast<typename T::value_type>(0)) {
        throw std::invalid_argument("Lambda must be positive for Poisson distribution.");
    }

    std::poisson_distribution<uint64_t> distribution_real(static_cast<uint64_t>(lambda.real()));
    std::poisson_distribution<uint64_t> distribution_imag(static_cast<uint64_t>(lambda.imag()));

    return T(static_cast<typename T::value_type>(distribution_real(rng)), static_cast<typename T::value_type>(distribution_imag(rng)));
};



template<AcceptedLike T>
class NoiseGenerator {
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

    NoiseGenerator(const NoiseType type) : type(type) {

        seed = pseudo_random_seed;

        randomNumberGen = std::make_unique<std::mt19937_64>(seed);

    }

    NoiseGenerator(const NoiseType type, const unsigned long long int seed) : seed(seed), type(type) {

        randomNumberGen = std::make_unique<std::mt19937_64>(seed);

    }

    template<AcceptedLike paramType>
    SeriesResult<T> jitter(const SeriesResult<T>& result, const paramType& tParam1, const paramType& tParam2 = T{}) {

        std::vector<T> newSn;
        std::vector<T> newAn;

        for (size_t i = 0; i < result.Sn.size(); ++i) {
            T noise;
            switch(type){
                case NoiseType::uniform:
                    noise = uniform(tParam1, tParam2);
                    break;
                case NoiseType::normal:
                    noise = normal(tParam1, tParam2);
                    break;
                case NoiseType::poisson:
                    noise = poisson(tParam1);
                    break;
            }
            if (i == 0) {
                newSn.push_back(result.Sn[0] + noise);
                newAn.push_back(newSn[0]);
            } else {
                newSn.push_back(result.Sn[i] + noise);
                newAn.push_back(newSn[i] - newSn[i - 1]);
            }
        }

        return SeriesResult<T>{.Sn = newSn, .an = newAn};
    };
    //tParam1 = low, tParam2 = high for uniform
    //tParam1 = mean, tParam2 = std for normal
    //tParam1 = lambda for poisson
};

#endif //SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP