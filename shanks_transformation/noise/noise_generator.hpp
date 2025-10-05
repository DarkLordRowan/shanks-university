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

enum noise_type {
    uniform,
    normal,
    poisson,
};


template<ComplexLike T>
void generate_uniform_noise(std::vector<T> &seq, const T bottom_border, const T top_border, const size_t seed) {
    if (bottom_border.real() >= top_border.real() || bottom_border.imag() >= top_border.imag()) {
        throw std::invalid_argument("Invalid borders for uniform noise generation.");
    }
    std::mt19937_64 rng(seed);
    double lo_r = static_cast<double>(bottom_border.real());
    double hi_r = static_cast<double>(top_border.real());
    double lo_i = static_cast<double>(bottom_border.imag());
    double hi_i = static_cast<double>(top_border.imag());

    std::uniform_real_distribution<double> dist_r(lo_r, hi_r);
    std::uniform_real_distribution<double> dist_i(lo_i, hi_i);

    if (!seq.empty()) {
        seq[0] = T(dist_r(rng), dist_i(rng));
        for (size_t i = 1; i < seq.size(); ++i) {
            seq[i] = seq[0] + T(dist_r(rng), dist_i(rng));
        }
    }
}

template<ComplexLike T>
void generate_normal_noise(std::vector<T> &seq, const T bottom_border, const T top_border, const size_t seed) {
    if (bottom_border.real() >= top_border.real() || bottom_border.imag() >= top_border.imag()) {
        throw std::invalid_argument("Invalid borders for normal noise generation.");
    }
    std::mt19937_64 rng(seed);
    double mean_r = (static_cast<double>(bottom_border.real()) + static_cast<double>(top_border.real())) / 2.0;
    double std_r = (static_cast<double>(top_border.real()) - static_cast<double>(bottom_border.real())) / 6.0;
    double mean_i = (static_cast<double>(bottom_border.imag()) + static_cast<double>(top_border.imag())) / 2.0;
    double std_i = (static_cast<double>(top_border.imag()) - static_cast<double>(bottom_border.imag())) / 6.0;

    std::normal_distribution<double> dist_r(mean_r, std_r);
    std::normal_distribution<double> dist_i(mean_i, std_i);

    if (!seq.empty()) {
        seq[0] = T(dist_r(rng), dist_i(rng));
        for (size_t i = 1; i < seq.size(); ++i) {
            seq[i] = seq[0] + T(dist_r(rng), dist_i(rng));
        }
    }
}

template<ComplexLike T>
void generate_poisson_noise(std::vector<T> &seq, const T bottom_border, const T top_border, const size_t seed) {
    throw std::invalid_argument("Invalid poisson noise generation.");
}

template<FloatLike T>
requires (!ComplexLike<T>)
void generate_uniform_noise(std::vector<T> &seq,const T bottom_border,const T top_border,const size_t seed) {
    if (bottom_border >= top_border) {
        throw std::invalid_argument("Invalid borders for uniform noise generation.");
    }
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<float_t> distribution(bottom_border, top_border);
    seq[0] = distribution(rng);
    for (size_t i = 1; i < seq.size(); ++i) {
        seq[i] = seq[0] + distribution(rng);
    }
};

template<FloatLike T>
requires (!ComplexLike<T>)
void generate_normal_noise(std::vector<T> &seq,const T bottom_border,const T top_border,const size_t seed) {
    if (bottom_border >= top_border) {
        throw std::invalid_argument("Invalid borders for normal noise generation.");
    }
    std::mt19937_64 rng(seed);
    T mean = (bottom_border + top_border) / static_cast<T>(2);
    T stddev = (top_border - bottom_border) / static_cast<T>(6);
    std::normal_distribution<float_t> distribution(mean, stddev);
    seq[0] = distribution(rng);
    for (size_t i = 1; i < seq.size(); ++i) {
        seq[i] = seq[0] + distribution(rng);
    }
};

template<FloatLike T>
requires (!ComplexLike<T>)
void generate_poisson_noise(std::vector<T> &seq,const T bottom_border,const T top_border,const size_t seed) {
    if (bottom_border < static_cast<T>(0) || top_border < static_cast<T>(0)) {
        throw std::invalid_argument("Poisson distribution requires non-negative borders.");
    }
    if (bottom_border >= top_border) {
        throw std::invalid_argument("Invalid borders for Poisson noise generation.");
    }
    std::mt19937_64 rng(seed);
    T mean = (bottom_border + top_border) / static_cast<T>(2);
    std::poisson_distribution<int> distribution(static_cast<int>(mean));
    seq[0] = static_cast<T>(distribution(rng));
    for (size_t i = 1; i < seq.size(); ++i) {
        seq[i] = seq[0] + static_cast<T>(distribution(rng));
    }
};



template<AcceptedLike T>
class Noise {
private:
    noise_type type;
    size_t seed;
    size_t size;

public:
    std::vector<T> seq;

    Noise() = delete;

    Noise(const size_t size, T bottom_border = static_cast<T>(-1.0), T top_border = static_cast<T>(1), const noise_type type = uniform,
        const size_t seed = std::chrono::system_clock::now().time_since_epoch().count() + std::rand()) : type(type), seed(seed), size(size) {

        seq = std::vector<T>(size);
        switch (type) {
            case uniform:
                generate_uniform_noise(seq, bottom_border, top_border, seed);
                break;
            case normal:
                generate_normal_noise(seq, bottom_border, top_border, seed);
                break;
            case poisson:
                generate_poisson_noise(seq, bottom_border, top_border, seed);
                break;
        }

    }

};

template<AcceptedLike T>
SeriesResult<T> jitter(const SeriesResult<T>& source, T bottom_border = static_cast<T>(-1.0), T top_border = static_cast<T>(1.0), noise_type type = uniform) {

    const size_t n = source.Sn->size();

    SeriesResult<T> result;
    result.Sn = std::make_shared<std::vector<T>>(*source.Sn);
    result.an = std::make_shared<std::vector<T>>(*source.an);

    if (n == 0) {
        return result;
    }

    Noise<T> noise(n, bottom_border, top_border, type);

    (*result.Sn)[0] += noise.seq[0];
    (*result.an)[0] += noise.seq[0];

    for (size_t j = 1; j < n; ++j) {
        (*result.Sn)[j] += noise.seq[j];
        (*result.an)[j] += (noise.seq[j] - noise.seq[j - 1]);
    }

    return result;
}
#endif //SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP