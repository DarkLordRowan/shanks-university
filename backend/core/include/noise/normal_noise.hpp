#ifndef NORMAL_NOISE_HPP
#define NORMAL_NOISE_HPP

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
    if (std <= 0) throw std::invalid_argument("Standard deviation must be positive for normal distribution.");

    // Creating normal distribution with specified parameters
    std::normal_distribution<float_t> distribution{mean, std};

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
    if (std.real() <= static_cast<typename T::value_type>(0) || std.imag() <= static_cast<typename T::value_type>(0))
        throw std::invalid_argument("Standard deviation must be positive for normal distribution.");

    // Generating real and imaginary parts independently with their own mean/std
    std::normal_distribution<float_t> distribution_real{utils::cast<float_t, typename real_of<T>::value>()(mean.real()),
                                                        utils::cast<float_t, typename real_of<T>::value>()(std.real())};
    std::normal_distribution<float_t> distribution_imag{utils::cast<float_t, typename real_of<T>::value>()(mean.imag()),
                                                        utils::cast<float_t, typename real_of<T>::value>()(std.imag())};

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
        std.leftinterval() <= static_cast<typename T::value_type>(0))
        throw std::invalid_argument("Standard deviation must be positive for normal distribution.");

    // Using left interval boundary for distribution generation
    std::normal_distribution<float_t> distribution{utils::cast<float_t, typename T::value_type>()(mean.leftinterval()),
                                                   utils::cast<float_t, typename T::value_type>()(std.leftinterval())};

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
    if (std <= utils::cast<T, int>()(0))
        throw std::invalid_argument("Standard deviation must be positive for normal distribution.");

    // Simple Gaussian distribution for scalar types
    std::normal_distribution<float_t> distribution{utils::cast<float_t, T>()(mean), utils::cast<float_t, T>()(std)};

    return utils::cast<T, float_t>()(distribution(rng));
}

#endif