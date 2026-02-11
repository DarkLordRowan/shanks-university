#ifndef POISSSON_NOISE_HPP
#define POISSSON_NOISE_HPP

template<typename T>
struct poisson_noise{
    static T generate(const double lambda, std::mt19937_64& rng){
        static_assert(std::false_type{}, "poisson_noise::generate isn't implemented for this type");
    }
};

template<typename T>
requires FloatLike<T> || IntervalLike<T>
struct poisson_noise<T>{
    static T generate(const double lambda, std::mt19937_64& rng);
};
template<typename T>
requires FloatLike<T> || IntervalLike<T>
T poisson_noise<T>::generate(const double lambda, std::mt19937_64& rng){
    std::poisson_distribution<unsigned int> distrb{lambda};
    return utils::cast<T, unsigned int>()(distrb(rng));
}

template<ComplexLike T>
struct poisson_noise<T>{
    static T generate(const double lambda, std::mt19937_64& rng);
};
template<ComplexLike T>
T poisson_noise<T>::generate(const double lambda, std::mt19937_64& rng){

    std::poisson_distribution<unsigned int> distrb{lambda};
    return utils::cast<T, unsigned int>()(distrb(rng), distrb(rng));

}

#endif