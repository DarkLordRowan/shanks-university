#pragma once

/**
 * @file console_IO.hpp
 * @brief This file contains console functions for different types of precisions.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

/**
 * @brief Structure for console input handling
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam InputType Type of input variable
 */
template <typename InputType>
struct console_IO {
    /**
     * @brief Generic input function with prompt for generic types
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param var_name (std::string)
     * @return InputType
     */
    static InputType input(const std::string& var_name = "x") {
        InputType x;
        // Reading from standard input
        std::cout << var_name << " : ";
        std::cin >> x;
        return x;
    }
};

/**
 * @brief Specialized console input for standard C++ types
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
template <typename InputType>
    requires std::floating_point<InputType> || std::unsigned_integral<InputType> || std::is_enum<InputType>::value
struct console_IO<InputType> {
    /**
     * @brief Input function with validation for standard types
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param var_name (std::string)
     * @return InputType
     */
    static InputType input(const std::string& var_name = "x");
};

// For interval precision types.
#ifdef INC_INTERVALPRECISION
template <FloatLike InputType>
struct console_IO<intprec::interval<InputType>> {
    static intprec::interval<InputType> input(const std::string& var_name = "x");
};
#endif

/**
 * @brief Specialization for std::complex<T> inputs
 */
template <std::floating_point T>
struct console_IO<std::complex<T>> {
    static std::complex<T> input(const std::string& var_name = "x");
};

/**
 * @brief Processing console inputs for different cases with error handling and validation.
 */
template <typename InputType>
    requires std::floating_point<InputType> || std::unsigned_integral<InputType> || std::is_enum<InputType>::value
InputType console_IO<InputType>::input(const std::string& var_name) {
    std::string init_string = "";
    InputType x = static_cast<InputType>(0);
    bool valid_input = false;
    bool error_triggered = false;
    const std::string error_mes_invalid_arg = "invalid input was given : ";
    const std::string error_mes_out_of_range = "given value is out of bounds for this type : ";

    // Error handling lambda for cleaner code
    auto error_handler = [&error_triggered, &init_string](const std::string& error_mes) {
        if (error_triggered) {
            console_effects::clear_lines_up(1);
        }
        console_effects::clear_lines_up(1);
        std::cout << error_mes << init_string << "\n";
        error_triggered = true;
    };

    // Loop until valid input is received
    while (!valid_input) {
        std::cout << var_name << " : ";
        std::getline(std::cin, init_string);

        try {
            // Converting string to appropriate numeric type
            if constexpr (std::is_same<InputType, float>::value) {
                x = std::stof(init_string);
            } else if constexpr (std::is_same<InputType, double>::value) {
                x = std::stod(init_string);
            } else if constexpr (std::is_same<InputType, long double>::value) {
                x = std::stold(init_string);
            } else if constexpr (std::is_same<InputType, unsigned long int>::value ||
                                 std::is_same<InputType, unsigned int>::value ||
                                 std::is_same<InputType, unsigned short int>::value) {
                x = static_cast<InputType>(std::stoul(init_string));
            } else if constexpr (std::is_same<InputType, unsigned long long int>::value) {
                x = std::stoull(init_string);
            } else if constexpr (std::is_enum<InputType>::value) {
                x = static_cast<InputType>(std::stoul(init_string));

                // Bound checks for enumerations
                if constexpr (std::is_same<InputType, shanks::series::series_id_t>::value) {
                    if (static_cast<size_t>(x) >= static_cast<size_t>(shanks::series::series_id_t::series_id_t_count))
                        throw std::out_of_range("Series with this id is not implemented");

                } else if constexpr (std::is_same<InputType, shanks::algos::transformation_id_t>::value) {
                    if (x >= shanks::algos::transformation_id_t::transformation_id_t_count)
                        throw std::out_of_range("Transformation with this id is not implemented");

                } else if constexpr (std::is_same<InputType, test_function_id_t>::value) {
                    if (x >= test_function_id_t::test_function_id_t_count)
                        throw std::out_of_range("Series with this id is not implemented");

                } else if constexpr (std::is_same<InputType, shanks::NoiseType>::value) {
                    if (x >= shanks::NoiseType::noise_count)
                        throw std::out_of_range("Series with this id is not implemented");
                }
            }

            valid_input = true;
        } catch (std::invalid_argument& e) {
            error_handler(error_mes_invalid_arg);
        } catch (std::out_of_range& e) {
            error_handler(error_mes_out_of_range);
        }
    }

    // Visual cleanup after error
    if (error_triggered) {
        console_effects::clear_lines_up(2);
        std::cout << var_name << " : " << init_string << "\n";
    }

    return x;
}

#ifdef INC_FPRECISION
/**
 * @brief Input implementation for float_precision type
 */
template <>
arb::float_precision console_IO<arb::float_precision>::input(const std::string& var_name) {
    std::string init_string = "";
    arb::float_precision x = 0;
    bool valid_input = false;
    bool error_triggered = false;
    const std::string error_mes_invalid_arg = "invalid input was given : ";
    const std::string error_mes_out_of_range = "given value is out of bounds for this type : ";
    auto error_handler = [&error_triggered, &init_string](const std::string& error_mes) {
        if (error_triggered) {
            console_effects::clear_lines_up(1);
        }
        console_effects::clear_lines_up(1);
        std::cout << error_mes << init_string << "\n";
        error_triggered = true;
    };

    while (!valid_input) {
        std::cout << var_name << " : ";
        std::getline(std::cin, init_string);

        try {
            x = arb::float_precision(init_string);
            valid_input = true;

        } catch (arb::float_precision::bad_int_syntax& e) {
            error_handler(error_mes_invalid_arg);
        } catch (arb::float_precision::bad_float_syntax& e) {
            error_handler(error_mes_invalid_arg);
        } catch (arb::float_precision::out_of_range& e) {
            error_handler(error_mes_out_of_range);
        }
    }

    if (error_triggered) {
        console_effects::clear_lines_up(2);
        std::cout << var_name << " : " << init_string << "\n";
    }

    // Requesting precision for the arbitrary precision type
    size_t precision = console_IO<size_t>::input("precision");

    return arb::float_precision(init_string, precision);
}
#endif

#ifdef INC_COMPLEXPRECISION
/**
 * @brief Input implementation for complex_precision types
 */
template <FloatLike InputType>
complex_precision<InputType> console_IO<complex_precision<InputType>>::input(const std::string& var_name) {
    // Separately reading real and imaginary parts
    InputType real_x = console_IO<InputType>::input("Re(x)");
    InputType imag_x = console_IO<InputType>::input("Im(x)");

    return complex_precision<InputType>(real_x, imag_x);
}
#endif

/**
 * @brief Input implementation for interval types
 */
template <FloatLike InputType>
intprec::interval<InputType> console_IO<intprec::interval<InputType>>::input(
    [[maybe_unused]] const std::string& var_name) {
    // Reading left and right boundaries of the interval
    InputType left_x = console_IO<InputType>::input("left border");
    InputType right_x = console_IO<InputType>::input("right border");

    return intprec::interval<InputType>(left_x, right_x);
}

#ifdef __MPREAL_H__
/**
 * @brief Input implementation for MPFR mpreal type
 */
template <>
mpfr::mpreal console_IO<mpfr::mpreal>::input(const std::string& var_name) {
    std::string float_value;
    std::cout << "Value of " << var_name << " : ";
    std::getline(std::cin, float_value);
    size_t precision = console_IO<size_t>::input("precision");
    mpfr::mpreal res = mpfr::mpreal(float_value, mpfr::digits2bits(precision));

    return res;
}

/**
 * @brief Input implementation for complex MPFR mpreal type
 */
template <>
std::complex<mpfr::mpreal> console_IO<std::complex<mpfr::mpreal>>::input(const std::string& var_name) {
    std::string real_value;
    std::cout << "Real of " << var_name << " : ";
    std::getline(std::cin, real_value);
    size_t real_precision = console_IO<size_t>::input("precision");
    std::string imag_value;
    std::cout << "Imag of " << var_name << " : ";
    std::getline(std::cin, imag_value);
    size_t imag_precision = console_IO<size_t>::input("precision");
    std::complex<mpfr::mpreal> res(mpfr::mpreal(real_value, mpfr::digits2bits(real_precision)),
                                   mpfr::mpreal(imag_value, mpfr::digits2bits(imag_precision)));

    return res;
}
#endif

template <std::floating_point T>
std::complex<T> console_IO<std::complex<T>>::input([[maybe_unused]] const std::string& var_name) {
    // Standard input for complex numbers via real and imaginary parts
    T real_value = console_IO<T>::input("Real of x");
    T imag_value = console_IO<T>::input("Imag of x");

    return std::complex<T>(real_value, imag_value);
}
