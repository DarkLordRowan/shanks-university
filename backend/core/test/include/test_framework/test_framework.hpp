/**
 * @file test_framework.hpp
 * @brief This file contains the framework for testing convergence acceleration of series.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

#pragma once

#include <limits.h>

#include <cstdio>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

// clang-format off
// Critical Section
#include "../interfaces/test_funcs_info.hpp"
#include "../interfaces/transformations_info.hpp"
#include "../console/console_effects.hpp"
#include "../console/console_IO.hpp"
#include "test_functions.hpp"
#include "test_mpfr.hpp"
// clang-format on

/**
 * @brief prints out all available series for testing
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
inline void print_series_info() {
    auto names = shanks::series::series_registry_metadata::get_names();

    std::cout << "Which series' convergence would you like to accelerate?" << '\n'
              << "List of currently available series:" << '\n';

    for (size_t i = 0; i < names.size(); ++i) {
        std::cout << i + 1 << " - " << names[i] << '\n';
    }
    std::cout << std::endl;
}

/**
 * @brief Factory function to create information objects for all available transformations
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @return std::vector<std::unique_ptr<ITransformationInfo>>
 */
inline std::vector<std::unique_ptr<ITransformationInfo>> create_transformation_info() {
    std::vector<std::unique_ptr<ITransformationInfo>> infos;
    auto names = shanks::algos::transformation_registry_metadata::get_names();
    auto ids = shanks::algos::transformation_registry_metadata::get_ids();

    for (size_t i = 0; i < names.size(); ++i) {
        infos.push_back(std::make_unique<DeclarativeTransformationInfo>(ids[i], names[i]));
    }
    return infos;
}

/**
 * @brief Factory function to create information objects for all available test functions
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @return std::vector<std::unique_ptr<ITestFunctionInfo>>
 */
inline std::vector<std::unique_ptr<ITestFunctionInfo>> create_test_function_info() {
    std::vector<std::unique_ptr<ITestFunctionInfo>> infos;
    auto names = test_function_registry_metadata::get_names();
    auto descriptions = test_function_registry_metadata::get_descriptions();
    auto ids = test_function_registry_metadata::get_ids();

    for (size_t i = 0; i < names.size(); ++i) {
        infos.push_back(std::make_unique<DeclarativeTestFunctionInfo>(ids[i], names[i], descriptions[i]));
    }
    return infos;
}

/**
 * @brief prints out all available transformations for testing
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
inline void print_transformation_info() {
    auto all_transformations = create_transformation_info();

    std::cout << "Which transformation would you like to test?" << '\n'
              << "List of currently available transformations:" << '\n';

    for (size_t i = 0; i < all_transformations.size(); ++i) {
        std::cout << i + 1 << " - " << all_transformations[i]->getName() << '\n';
    }
    std::cout << '\n';
}

/**
 * @brief prints out all available functions for testing
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
inline void print_test_function_info() {
    auto all_functions = create_test_function_info();

    std::cout << "Which function would you like to use for testing?" << '\n'
              << "List of currently available functions:" << '\n';

    for (size_t i = 0; i < all_functions.size(); ++i) {
        std::cout << i + 1 << " - " << all_functions[i]->getName() << " - " << all_functions[i]->getDescription()
                  << '\n';
    }
    std::cout << '\n';
}

/**
 * @brief Helper function to get transformation by ID
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param id (shanks::algos::transformation_id_t)
 * @return std::unique_ptr<shanks::algos::series_acceleration<T, K>>
 * @throws std::domain_error if transformation ID is invalid
 */
template <AcceptedLike T, std::unsigned_integral K>
inline std::unique_ptr<shanks::algos::series_acceleration<T, K>> create_transformation_by_id(
    shanks::algos::transformation_id_t id) {
    return shanks::algos::transformation_registry<T, K>::create(id);
}

/**
 * @brief The main testing function
 * This function provides a convenient and interactive way to test out the convergence acceleration of various series
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */

template <AcceptedLike T, std::unsigned_integral K>
inline void main_testing_function() {
    // User interaction to select series
    print_series_info();
    size_t series_idx = console_IO<size_t>::input("series_index") - 1;

    // User input for series argument x
    std::cout << "Enter x - the argument for the functional series" << '\n';
    T x = console_IO<T>::input();

    // Optional parameters for specialized series (e.g., alpha for IncompleteGamma)
    const auto& entry = shanks::series::series_registry<T, K>::get_entries()[series_idx];
    T tParam = utils::cast<T, int>()(1);
    K kParam = static_cast<K>(1);

    if (entry.needsT || entry.needsK) {
        std::cout << "Enter additional parameters for " << entry.name << ":" << '\n';
        if (entry.needsT) tParam = console_IO<T>::input("tParam (parameter/alpha)");
        if (entry.needsK) kParam = console_IO<K>::input("kParam (order/m)");
    }

    std::unique_ptr<shanks::series::series_base<T, K>> series =
        shanks::series::series_registry<T, K>::create(series_idx, x, tParam, kParam);

    std::cout << "\nChosen series: " << entry.name << "\n";
    // User interaction to select transformation algorithm
    print_transformation_info();
    size_t transform_idx = console_IO<size_t>::input("transformation_idx") - 1;
    auto transform_ids = shanks::algos::transformation_registry_metadata::get_ids();
    if (transform_idx >= transform_ids.size()) throw std::out_of_range("Invalid transformation index");

    shanks::algos::transformation_id_t transformation_id = transform_ids[transform_idx];
    std::unique_ptr<shanks::algos::series_acceleration<T, K>> transform =
        create_transformation_by_id<T, K>(transformation_id);

    // User interaction to select test function
    print_test_function_info();
    size_t func_idx = console_IO<size_t>::input("function_idx") - 1;
    auto func_ids = test_function_registry_metadata::get_ids();
    if (func_idx >= func_ids.size()) throw std::out_of_range("Invalid function index");
    test_function_id_t function_id = func_ids[func_idx];

    std::cout << "Enter n and order:" << '\n';
    K n = console_IO<K>::input("n");
    K order = console_IO<K>::input("order");
    // Generating the initial series terms

    series_result<T> result = series->generate(n + 3 * order + 1);

    // Optional noise application
    std::string answer = "ok";
    while (answer != "Y" && answer != "Yes" && answer != "N" && answer != "No") {
        std::cout << "Appply noise [Y]es, [N]o: ";
        std::getline(std::cin, answer);
    }

    if (answer == "Y" || answer == "Yes") {
        std::cout << "Noise type: uniform[0], normal[1], poisson[2]\n";
        NoiseType noise_type_to_use = console_IO<NoiseType>::input("noise_type");

        result = apply_noise_interactive<T, typename GetUnderlyingType<T>::value>(result, noise_type_to_use);
    }

    // Executing the selected test function
    switch (function_id) {
        case test_function_id_t::cmp_sum_and_transform_id:
            cmp_sum_and_transform<T, K>(n, order, series.get(), result, transform.get());
            break;
        case test_function_id_t::cmp_a_n_and_transform_id:
            cmp_a_n_and_transform<T, K>(n, order, result, transform.get());
            break;
        case test_function_id_t::transformation_remainder_id:
            transformation_remainders<T, K>(n, order, series.get(), result, transform.get());
            break;
        case test_function_id_t::cmp_transformations_id: {
            print_transformation_info();
            size_t cmp_transform_idx = console_IO<size_t>::input("transformation_idx") - 1;
            if (cmp_transform_idx >= transform_ids.size()) throw std::out_of_range("Invalid transformation index");
            shanks::algos::transformation_id_t cmp_transformation_id = transform_ids[cmp_transform_idx];
            std::unique_ptr<shanks::algos::series_acceleration<T, K>> transform2;

            transform2 = create_transformation_by_id<T, K>(cmp_transformation_id);
            cmp_transformations<T, K>(n, order, series.get(), result, transform.get(), transform2.get());
            break;
        }
        case test_function_id_t::eval_transform_time_id:
            eval_transform_time<T, K>(n, order, result, transform.get());
            break;
        case test_function_id_t::test_all_transforms_id: {
            test_all_transforms<T, K>(n, order, result);
            break;
        }
        default:
            throw std::domain_error("wrong function_id");
    }
}

#include "test_series.hpp"