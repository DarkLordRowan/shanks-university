#include "../base_noise_gen.hpp"

#include <chrono>
#include <random>

template<AcceptedLike T>
class UniformNoiseGenerator : public BaseNoiseGenerator<T>{
protected:

    std::mt19937_64 pseudoRNG;
    std::uniform_real_distribution<double> uniformRNG;

public:

    UniformNoiseGenerator(const T& a, const T& b) {

        BaseNoiseGenerator<T>::seed = std::chrono::system_clock::now().time_since_epoch().count() + std::rand();
        pseudoRNG = std::mt19937_64(BaseNoiseGenerator<T>::seed);
        uniformRNG = std::uniform_real_distribution<double>(a, b); //only for defaults


    };

    T operator()(size_t index){
        return static_cast<T>(uniformRNG(pseudoRNG));
    }

};

template<>
class UniformNoiseGenerator<complex_precision<float_precision>> : public BaseNoiseGenerator<complex_precision<float_precision>> {
protected:

    std::mt19937_64 pseudoRNG;
    std::uniform_real_distribution<double> uniformRNGreal;
    std::uniform_real_distribution<double> uniformRNGimag;

public:

    UniformNoiseGenerator(const complex_precision<float_precision>& a, const complex_precision<float_precision>& b) {

        BaseNoiseGenerator<complex_precision<float_precision>>::seed = std::chrono::system_clock::now().time_since_epoch().count() + std::rand();
        pseudoRNG = std::mt19937_64(BaseNoiseGenerator<complex_precision<float_precision>>::seed);
        uniformRNGreal = std::uniform_real_distribution<double>(static_cast<double>(a.real()), static_cast<double>(b.real()));
        uniformRNGimag = std::uniform_real_distribution<double>(static_cast<double>(a.imag()), static_cast<double>(b.imag()));


    };

    complex_precision<float_precision> operator()(size_t index){
        return complex_precision<float_precision>(uniformRNGreal(pseudoRNG), uniformRNGimag(pseudoRNG));
    }
};