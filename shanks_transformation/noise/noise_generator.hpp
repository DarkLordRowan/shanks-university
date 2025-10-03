//
// Created by user on 03.10.2025.
//
#pragma once

#include <chrono>
#include "../custom_concepts.hpp"
#include <random>
#include <vector>

#ifndef SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP
#define SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP

enum noise_type {
    uniform,
    normal,
    poisson,
};

template<typename T>
void generate_uniform_noise(std::vector<T> &seq,const T bottom_border,const T top_border,const size_t seed) {
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<float_t> distribution(bottom_border, top_border);
    seq[0] = distribution(rng);
    for (size_t i = 1; i < seq.size(); ++i) {
        seq[i] = seq[0] + distribution(rng);
    }
};

template<typename T>
void generate_normal_noise(std::vector<T> &seq,const T bottom_border,const T top_border,const size_t seed) {
    std::mt19937_64 rng(seed);
    T mean = (bottom_border + top_border) / static_cast<T>(2);
    T stddev = (top_border - bottom_border) / static_cast<T>(6);
    std::normal_distribution<float_t> distribution(mean, stddev);
    seq[0] = distribution(rng);
    for (size_t i = 1; i < seq.size(); ++i) {
        seq[i] = seq[0] + distribution(rng);
    }
};

template<typename T>
void generate_poisson_noise(std::vector<T> &seq,const T bottom_border,const T top_border,const size_t seed) {
    std::mt19937_64 rng(seed);
    T mean = (bottom_border + top_border) / static_cast<T>(2);
    std::poisson_distribution<int> distribution(static_cast<int>(mean));
    seq[0] = static_cast<T>(distribution(rng));
    for (size_t i = 1; i < seq.size(); ++i) {
        seq[i] = seq[0] + static_cast<T>(distribution(rng));
    }
};

template<typename T>
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

        if (bottom_border >= top_border) {
            throw std::invalid_argument("Bottom border must be less than top border");
        }

        if (type == poisson && (bottom_border < static_cast<T>(0) || top_border < static_cast<T>(0))) {
            throw std::invalid_argument("Poisson distribution requires non-negative borders");
        }

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

#endif //SHANKS_TRANSFORMATION_NOISE_GENERATOR_HPP