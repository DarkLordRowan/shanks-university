#ifndef UNIFORM_NOISE_HPP
#define UNIFORM_NOISE_HPP

template <typename T>
struct uniform_noise {
    static T generate(const T& left, const T& right, std::mt19937_64& rng) {
        static_assert(std::false_type{}, "uniform_noise::generate isn't implemented for this type");
    }
};

template <typename T>
struct uniform_noise<shanks::profiling::OperationCounting<T>> {
    static shanks::profiling::OperationCounting<T> generate(const shanks::profiling::OperationCounting<T>& left,
                                                            const shanks::profiling::OperationCounting<T>& right,
                                                            std::mt19937_64& rng) {
        return shanks::profiling::OperationCounting<T>(uniform_noise<T>::generate(left.value, right.value, rng));
    }
};

template <std::floating_point T>
struct uniform_noise<T> {
    static T generate(const T& left, const T& right, std::mt19937_64& rng);
};
template <std::floating_point T>
T uniform_noise<T>::generate(const T& left, const T& right, std::mt19937_64& rng) {
    std::uniform_real_distribution<T> distrb{left, right};
    return distrb(rng);
}

#ifdef __MPREAL_H__
template <>
struct uniform_noise<mpfr::mpreal> {
    static mpfr::mpreal generate(const mpfr::mpreal& left, const mpfr::mpreal& right, std::mt19937_64& rng);
};

inline mpfr::mpreal uniform_noise<mpfr::mpreal>::generate(const mpfr::mpreal& left, const mpfr::mpreal& right,
                                                          std::mt19937_64& rng) {
    std::uniform_real_distribution<long double> distrb{0.0L, 1.0L};
    return left + (right - left) * mpfr::mpreal(distrb(rng), utils::helpers<mpfr::mpreal>::get_precision(left));
}

#endif

template <typename T>
    requires ComplexLike<T> || IntervalLike<T>
struct uniform_noise<T> {
    static T generate(const typename GetUnderlyingType<T>::value left, const typename GetUnderlyingType<T>::value right,
                      std::mt19937_64& rng);
};

template <typename T>
    requires ComplexLike<T> || IntervalLike<T>
T uniform_noise<T>::generate(const typename GetUnderlyingType<T>::value left,
                              const typename GetUnderlyingType<T>::value right, std::mt19937_64& rng) {
    using NextT = typename GetUnderlyingType<T>::value;
    using BaseOfNextT = typename GetUnderlyingType<NextT>::value;
    
    if constexpr (IntervalLike<NextT>) {
        return utils::cast<T, NextT>()(
            uniform_noise<NextT>::generate(left.mid(), right.mid(), rng));
    } else {
        return utils::cast<T, NextT>()(
            uniform_noise<NextT>::generate(utils::cast<BaseOfNextT, NextT>()(left), 
                                          utils::cast<BaseOfNextT, NextT>()(right), rng));
    }
}

#endif