#ifndef NORMAL_NOISE_HPP
#define NORMAL_NOISE_HPP

template<typename T>
struct normal_noise{
    static T generate(const double mean, const double var, std::mt19937_64& rng){
        static_assert(std::false_type{}, "normal_noise::generate isn't implemented for this type");
    }
};

template<typename T>
requires FloatLike<T> || IntervalLike<T>
struct normal_noise<T>{
    static T generate(const double mean, const double var, std::mt19937_64& rng);
};
template<typename T>
requires FloatLike<T> || IntervalLike<T>
T normal_noise<T>::generate(const double mean, const double var, std::mt19937_64& rng){
    std::normal_distribution<long double> distrb{mean, var};
    return utils::cast<T, long double>()(distrb(rng));
}

template<ComplexLike T>
struct normal_noise<T>{
    static T generate(const double mean, const double var, std::mt19937_64& rng);
};
template<ComplexLike T>
T normal_noise<T>::generate(const double mean, const double var, std::mt19937_64& rng){

    std::normal_distribution<long double> distrb{mean, var};
    return utils::cast<T, long double>()(distrb(rng), distrb(rng));

}

#endif