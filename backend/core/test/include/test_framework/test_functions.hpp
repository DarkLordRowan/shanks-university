/**
 * @file test_functions.hpp
 * @brief This file contains the implementation of various testing functions.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

#pragma once

#include <chrono>

/**
 * @brief Function that prints out comparison between transformed and nontransformed partial sums
 * At first it prints out the type of transformation, series that are being transformed, type of enumerating integer and
 * type of series terms Then it prints out partial sums of first i terms of the series where i ranges from 1 to n (!)
 * After that it prints out transformed partial sum of first i terms of the series of order order
 * At last it prints out the difference between the two
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of
 * transformation we are using
 * @param n The number of terms
 * @param order The order of the transformation
 * @param series The series class object to be accelerated
 * @param test The type of transformation that is being used
 */
template <AcceptedLike T, std::unsigned_integral K>
void cmp_sum_and_transform(const K n, const K order, shanks::series::series_base<T, K>* series,
                           series_result<T>& result, shanks::algos::series_acceleration<T, K>* test) {
    std::cout << "transformation : " << test->get_name() << "\n";
    for (K i = 1; i <= n; ++i) {
        try {
            // Calculating transformed value
            const T res = test->operator()(i, order, result);
            // Displaying comparison details
            std::cout << "Sum of algo : " << utils::helpers<T>::to_string(series->get_sum()) << '\n';
            std::cout << "S_" << i << " : " << utils::helpers<T>::to_string(result.Sn[i]) << "\n";
            std::cout << "T_" << i << " of order " << order << " : " << utils::helpers<T>::to_string(res) << std::endl;
            std::cout << "T_" << i << " of order " << order << " - S_" << i << " : "
                      << utils::helpers<T>::to_string(res - result.Sn[i]) << '\n';

        } catch (std::domain_error& e) {
            std::cout << e.what() << '\n';
        } catch (std::overflow_error& e) {
            std::cout << e.what() << '\n';
        }
    }
}

/**
 * @brief Function that prints out comparison between the terms of transformed and nontransformed series
 * At first it prints out the type of transformation, series that are being transformed, type of enumerating integer and
 * type of series terms Then it prints out terms from the first to nth of the series At last it prints out terms from
 * the first to nth of the transformed series
 * @authors Naumov A.U. , Lykov D.S.,  Kreynin R.G.
 * @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of
 * transformation we are using
 * @param n The number of terms
 * @param order The order of the transformation
 * @param series The series class object to be accelerated
 * @param test The type of transformation that is being used
 */
template <AcceptedLike T, std::unsigned_integral K>
void cmp_a_n_and_transform(const K n, const K order, series_result<T>& result,
                           shanks::algos::series_acceleration<T, K>* test) {
    std::cout << "transformation : " << test->get_name() << "\n";
    for (K i = 1; i <= n; ++i) {
        try {
            // Displaying original terms
            std::cout << "a_" << i << " : " << utils::helpers<T>::to_string(result.an[i]) << '\n';
            // Calculating and displaying transformed terms
            std::cout << "t_" << i << " : "
                      << utils::helpers<T>::to_string(test->operator()(i, order, result) -
                                                      test->operator()(i - 1, order, result))
                      << '\n';
            std::cout << "t_" << i << " of order " << order << " - a_" << i << " : "
                      << utils::helpers<T>::to_string(
                             (test->operator()(i, order, result) - test->operator()(i - 1, order, result)) -
                             result.an[i])
                      << '\n';
        } catch (std::domain_error& e) {
            std::cout << e.what() << '\n';
        } catch (std::overflow_error& e) {
            std::cout << e.what() << '\n';
        }
    }
}

/**
 * @brief Applies noise to a given series
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param source (series_result<T>), noise_type (NoiseType)
 * @return series_result<T> (noisy result)
 * @tparam T The type of the elements in the series
 * @tparam ParamType The type of the noise parameters
 *
 * @param source The original series to which noise will be applied
 * Valid values: any series_result<T>.
 * @param noise_type The type of noise to apply
 * Valid values: NoiseType::uniform, NoiseType::normal, NoiseType::poisson.
 * @param param1 The first parameter for the noise generation.\n
 * Valid values: depends on noise type:
 * -For uniform noise: Any real number representing the lower bound or complex number with real and imaginary parts as
 * lower bounds. -For normal noise: Any real number representing the mean or complex number with real and imaginary
 * parts as means. -For poisson noise: Any positive real number representing the lambda parameter.
 * @param param2 The second parameter for the noise generation (optional, depending on noise type)\n
 * Valid values: depends on noise type:
 * -For uniform noise: Any real number greater than param1 representing the upper bound or complex number with real and
 * imaginary parts greater than those of param1. -For normal noise: Any positive real number representing the standard
 * deviation or complex number with positive real and imaginary parts. -For poisson noise: Not used, can be default
 * constructed.
 * @param seed The seed for the random number generator (default is based on current time and random value)\n
 * Valid values: any positive integer.
 *
 * @return A new series_result<T> with noise applied
 */
template <AcceptedLike T, FloatLike ParamType>
inline series_result<T> apply_noise_interactive(series_result<T>& source, const shanks::NoiseType noise_type) {
    ParamType param1;
    ParamType param2;
    unsigned long long seed;

    // Getting seed from user
    std::cout << "Enter seed (0 for random, any other positive integer will be used as seed)\n";
    seed = console_IO<unsigned long long int>::input("seed");

    if (seed == 0) {
        seed = std::chrono::system_clock::now().time_since_epoch().count() + std::rand();
    }

    std::cout << "Seed : " << utils::helpers<unsigned long long>::to_string(seed) << "\n";

    // Getting noise parameters based on type
    switch (noise_type) {
        case shanks::uniform:
            param1 = console_IO<ParamType>::input("Lower bound");
            param2 = console_IO<ParamType>::input("Upper bound");
            break;
        case shanks::normal:
            param1 = console_IO<ParamType>::input("Mean");
            param2 = console_IO<ParamType>::input("Standard deviation");
            break;
        case shanks::poisson:
            param1 = console_IO<ParamType>::input("Lambda");
            param2 = ParamType();  // Not used for poisson
            break;
        default:
            throw std::invalid_argument("Invalid noise type");
    }

    // Applying noise
    switch (noise_type) {
        case shanks::uniform:
            return shanks::apply_uniform_noise(source, shanks::NoiseMethod::jitter, seed,
                                               utils::cast<double, ParamType>()(param1),
                                               utils::cast<double, ParamType>()(param2));
        case shanks::normal:
            return shanks::apply_normal_noise(source, shanks::NoiseMethod::jitter, seed,
                                              utils::cast<double, ParamType>()(param1),
                                              utils::cast<double, ParamType>()(param2));
        case shanks::poisson:
            return shanks::apply_poisson_noise(source, shanks::NoiseMethod::jitter, seed,
                                               utils::cast<double, ParamType>()(param1));
        default:
            throw std::invalid_argument("Invalid noise type");
    }
}

/**
 * @brief Function that prints out the remainders
 * At first it prints out the type of transformation, series that are being transformed, type of enumerating integer and
 * type of series terms Then it prints out remainders of the series from 1 to n
 * @authors Naumov A.U. , Lykov D.S.,  Kreynin R.G.
 * @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of
 * transformation we are using
 * @param n The number of terms for the last remainder
 * @param order The order of the transformation
 * @param series The series class object to be accelerated
 * @param test The type of transformation that is being used
 */
template <AcceptedLike T, std::unsigned_integral K>
void transformation_remainders(const K n, const K order, shanks::series::series_base<T, K>* series,
                               series_result<T>& result, shanks::algos::series_acceleration<T, K>* test) {
    std::cout << "Tranformation of order " << order << " remainders from i = 1 to " << n << '\n';
    std::cout << "transformation : " << test->get_name() << "\n";
    for (K i = 1; i <= n; ++i) {
        try {
            // Calculating and displaying remainder: Sum - Transformed_Sum
            std::cout << "S - T_" << i << " : "
                      << utils::helpers<T>::to_string(series->get_sum() - test->operator()(i, order, result)) << '\n';
        } catch (std::domain_error& e) {
            std::cout << e.what() << '\n';
        } catch (std::overflow_error& e) {
            std::cout << e.what() << '\n';
        }
    }
}

/**
 * @brief Function that showcases the difference between 2 transformations
 * At first it prints out the type of transformations, series that are being transformed, type of enumerating integer
 * and type of series terms Then it prints out remainders of the series sum from 1 to n transformed 2 different ways and
 * also tells which one got closer to the sum of the series
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam series_templ is the type of series whose convergence we accelerate, transform_type_1 is the first type of
 * transformation we are using, transform_type_2 is the second type of transformation we are using
 * @param n The number of terms for the last remainder
 * @param order The order of the transformation
 * @param series The series class object to be accelerated
 * @param test_1 The type of the first transformation that is being used
 * @param test_2 The type of the second transformation that is being used
 */
template <AcceptedLike T, std::unsigned_integral K>
void cmp_transformations(const K n, const K order, shanks::series::series_base<T, K>* series, series_result<T>& result,
                         shanks::algos::series_acceleration<T, K>* test_1,
                         shanks::algos::series_acceleration<T, K>* test_2) {
    std::cout << "Tranformations of order " << order << " remainders from i = 1 to " << n << '\n';
    std::cout << "The transformation #1 is ";
    std::cout << test_1->get_name() << "\n";
    std::cout << "The transformation #2 is ";
    std::cout << test_2->get_name() << "\n";

    T diff_1;
    T diff_2;
    for (K i = 1; i <= n; ++i) {
        try {
            // Calculating remainders for both transformations
            diff_1 = series->get_sum() - test_1->operator()(i, order, result);
            diff_2 = series->get_sum() - test_2->operator()(i, order, result);

            // Displaying remainders
            std::cout << "The transformation #1: S - T_" << i << " : " << utils::helpers<T>::to_string(diff_1) << '\n';
            std::cout << "The transformation #2: S - T_" << i << " : " << utils::helpers<T>::to_string(diff_2) << '\n';

            // Comparing absolute errors
            if (utils::math<T>::abs(diff_1) < utils::math<T>::abs(diff_2))
                std::cout << "The transformation #1 is faster" << '\n';
            else
                std::cout << "The transformation #2 is faster" << '\n';
        } catch (std::domain_error& e) {
            std::cout << e.what() << '\n';
        } catch (std::overflow_error& e) {
            std::cout << e.what() << '\n';
        }
    }
}

/**
 * @brief Function that evaluates the time it takes to transform series
 * @authors Naumov A.U. , Lykov D.S.,  Kreynin R.G.
 * @tparam series_templ is the type of series whose convergence we accelerate, transform_type is the type of
 * transformation we are using
 * @param n The number of terms for the last remainder
 * @param order The order of the transformation
 * @param series The series class object to be accelerated
 * @param test The type of the first transformation that is being used
 */
template <AcceptedLike T, std::unsigned_integral K>
void eval_transform_time(const K n, const K order, series_result<T>& result,
                         shanks::algos::series_acceleration<T, K>* test) {
    // Starting timer
    const auto start_time = std::chrono::system_clock::now();
    std::cout << "transformation : " << test->get_name() << "\n";
    for (K i = 1; i <= n; ++i) {
        try {
            // Performing transformation
            test->operator()(i, order, result);
        } catch (std::domain_error& e) {
            std::cout << e.what() << '\n';
        } catch (std::overflow_error& e) {
            std::cout << e.what() << '\n';
        }
    }
    // Ending timer
    const auto end_time = std::chrono::system_clock::now();
    const std::chrono::duration<double, std::milli> diff = end_time - start_time;
    std::cout << "It took " << diff.count() << " to perform these transformations" << '\n';
}

/**
 * @brief Function that tests all available transformations on a series
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param n (K), order (K), series (series_base*), result (series_result&)
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 */
template <AcceptedLike T, std::unsigned_integral K>
void test_all_transforms(const K n, const K order, series_result<T>& result) {
    // Get all registered transformations
    auto entries = shanks::algos::transformation_registry<T, K>::get_entries();
    std::vector<std::unique_ptr<shanks::algos::series_acceleration<T, K>>> algos;
    for (const auto& entry : entries) {
        algos.push_back(entry.factory(""));
    }

    // Running tests for each algorithm
    for (size_t i = 0; i <= n; ++i) {
        for (size_t j = 0; j < algos.size(); ++j) {
            try {
                std::cout << "transformation : " << algos[j]->get_name() << "\n";
                std::cout << "n = " << i << " order = " << order << " : "
                          << utils::helpers<T>::to_string(algos[j]->operator()(i, order, result)) << "\n\n";
            } catch (std::overflow_error& e) {
                std::cout << e.what() << "\n";
            } catch (std::domain_error& e) {
                std::cout << e.what() << "\n";
            } catch (std::out_of_range& e) {
                std::cout << e.what() << "\n";
            } catch (std::invalid_argument& e) {
                std::cout << e.what() << "\n";
            } catch (std::runtime_error& e) {
                std::cout << e.what() << "\n";
            } catch (...) {
                std::cout << "SOME ERROR\n";
            }
        }
        std::cout << "\n";
    }
}
