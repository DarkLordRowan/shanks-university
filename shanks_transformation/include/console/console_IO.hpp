#pragma once

#include <concepts>
#include <iostream>
#include <string>

#include "../custom_concepts.hpp"

#include "../series.hpp"
#include "../methods.hpp"
#include "../interfaces/test_funcs_info.hpp"
#include "../noise/noise_generator.hpp"

#include "console_effects.hpp"


template<typename InputType>
struct console_IO{

	InputType inline static input(const std::string& var_name  = "x"){
		InputType x;
		std::cout << var_name <<" : "; std::cin >> x;
		return x;
	}
};

//FOR STANDART

template<typename InputType>
requires std::floating_point<InputType> || std::unsigned_integral<InputType> || std::is_enum<InputType>::value
struct console_IO<InputType>{

	InputType inline static input(const std::string& var_name  = "x");
};

//FOR ARB PRECISION
#ifdef INC_FPRECISION
template<>
struct console_IO<float_precision> {
	float_precision inline static input(const std::string& var_name = "x");
};

#ifdef INC_COMPLEXPRECISION
//FOR COMPLEX PREC
template<FloatLike InputType>
struct console_IO<complex_precision<InputType>> {

	complex_precision<InputType> inline static input(const std::string& var_name = "x");
};
#endif
#endif
//FOR INTERVAL PREC

#ifdef INC_INTERVALPRECISION
template<FloatLike InputType>
struct console_IO<interval<InputType>> {
	interval<InputType> inline static input(const std::string& var_name = "x");
};
#endif

template<typename InputType>
requires std::floating_point<InputType> || std::unsigned_integral<InputType> || std::is_enum<InputType>::value
InputType inline console_IO<InputType>::input(const std::string& var_name){

    std::string init_string = "";
    InputType x = static_cast<InputType>(0);
    bool valid_input = false;
	bool error_triggered = false;
    const std::string error_mes_invalid_arg = "invalid input was given : ";
    const std::string error_mes_out_of_range = "given value is out of bounds for this type : ";
	auto error_handler = [&error_triggered, &init_string](const std::string& error_mes){
		if (error_triggered){
				console_effects::clear_lines_up(1);
			}
			console_effects::clear_lines_up(1);
            std::cout << error_mes << init_string << "\n";
			error_triggered = true;
	};

    while(!valid_input){

        std::cout << var_name <<" : "; std::getline(std::cin, init_string);

        try{
            if constexpr (std::is_same<InputType, float>::value){
                x = std::stof(init_string);
            } else if constexpr (std::is_same<InputType, double>::value){
                x = std::stod(init_string);
            } else if constexpr (std::is_same<InputType, long double>::value) {
                x = std::stold(init_string);
			} else if constexpr (
				std::is_same<InputType, unsigned long int>::value || 
				std::is_same<InputType, unsigned int>::value 	  ||
				std::is_same<InputType, unsigned short int>::value
			) {
                x = static_cast<InputType>(std::stoul(init_string));
            } else if constexpr (std::is_same<InputType, unsigned long long int>::value){
                x = std::stoull(init_string);
            } else if constexpr (std::is_enum<InputType>::value){

				x = static_cast<InputType>(std::stoul(init_string));

                if constexpr (std::is_same<InputType, series_id_t>::value){

                    if(x >= series_id_t::series_id_t_count) throw std::out_of_range("Series with this id is not implemented");

                } else if constexpr (std::is_same<InputType, transformation_id_t>::value){

                    if(x >= transformation_id_t::transformation_id_t_count) throw std::out_of_range("Series with this id is not implemented");

                } else if constexpr (std::is_same<InputType, test_function_id_t>::value){

                    if(x >= test_function_id_t::test_function_id_t_count) throw std::out_of_range("Series with this id is not implemented");

                } else if constexpr (std::is_same<InputType, NoiseType>::value){

                    if(x >= NoiseType::noise_count) throw std::out_of_range("Series with this id is not implemented");

                }
				
			}

            valid_input = true;
        } 
		catch (std::invalid_argument& e){ error_handler(error_mes_invalid_arg); }  
		catch (std::out_of_range& e) { error_handler(error_mes_out_of_range); }
    }

	if(error_triggered){
		console_effects::clear_lines_up(2);
		std::cout << var_name << " : " << init_string << "\n";
	}

    return x;

}

#ifdef INC_FPRECISION
float_precision inline console_IO<float_precision>::input(const std::string& var_name){

    std::string init_string = "";
    float_precision x = 0;
    bool valid_input = false;
	bool error_triggered = false;
    const std::string error_mes_invalid_arg = "invalid input was given : ";
    const std::string error_mes_out_of_range = "given value is out of bounds for this type : ";
	auto error_handler = [&error_triggered, &init_string](const std::string& error_mes){
		if (error_triggered){
				console_effects::clear_lines_up(1);
			}
			console_effects::clear_lines_up(1);
            std::cout << error_mes << init_string << "\n";
			error_triggered = true;
	};
    
    while(!valid_input){

        std::cout << var_name <<" : "; std::getline(std::cin, init_string);

        try{

            x = float_precision(init_string);
            valid_input = true;

        } 
        catch (float_precision::bad_int_syntax& e){ error_handler(error_mes_invalid_arg); } 
		catch (float_precision::bad_float_syntax& e){ error_handler(error_mes_invalid_arg); }
        catch (float_precision::out_of_range& e) { error_handler(error_mes_out_of_range); }
    }

	if(error_triggered){
		console_effects::clear_lines_up(2);
		std::cout << var_name << " : " << init_string << "\n";
	}

    size_t precision = console_IO<size_t>::input("precision");
    x.precision(precision);

    return x;

}
#endif

#ifdef INC_COMPLEXPRECISION
template<FloatLike InputType>
complex_precision<InputType> inline console_IO<complex_precision<InputType>>::input(const std::string& var_name){

    InputType real_x = console_IO<InputType>::input("Re(x)");
    InputType imag_x = console_IO<InputType>::input("Im(x)");

    return complex_precision<InputType>(real_x, imag_x);

}
#endif

#ifdef INC_INTERVALPRECISION
template<FloatLike InputType>
interval<InputType> inline console_IO<interval<InputType>>::input(const std::string& var_name){

    InputType left_x = console_IO<InputType>::input("left border");
    InputType right_x = console_IO<InputType>::input("right border");

    return interval<InputType>(left_x, right_x);

}
#endif

#ifdef _CL_FLOAT_CLASS_H
template<>
cln::cl_F inline console_IO<cln::cl_F>::input(const std::string& var_name){

    std::string float_value = console_IO<std::string>::input();
    size_t precision = console_IO<size_t>::input("precision");
    cln::cl_F res = (float_value + "_" + utils::to_string(precision)).c_str();

    return res;

}
#endif

#ifdef __MPREAL_H__
template<>
mpfr::mpreal inline console_IO<mpfr::mpreal>::input(const std::string& var_name){

    std::string float_value; std::getline(std::cin, float_value);
    size_t precision = console_IO<size_t>::input("precision");
    mpfr::mpreal res = mpfr::mpreal(float_value, mpfr::digits2bits(precision));

    return res;
}

template<>
std::complex<mpfr::mpreal> inline console_IO<std::complex<mpfr::mpreal>>::input(const std::string& var_name){

    std::string real_value; std::getline(std::cin, real_value);
    size_t real_precision = console_IO<size_t>::input("precision");
    std::string imag_value; std::getline(std::cin, imag_value);
    size_t imag_precision = console_IO<size_t>::input("precision");
    std::complex<mpfr::mpreal> res(
        mpfr::mpreal(real_value, mpfr::digits2bits(real_precision)),
        mpfr::mpreal(imag_value, mpfr::digits2bits(imag_precision))
    );

    return res;
}
#endif