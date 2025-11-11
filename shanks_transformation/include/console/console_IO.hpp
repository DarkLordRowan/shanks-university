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
template<>
struct console_IO<float_precision> {

	float_precision inline static input(const std::string& var_name = "x");
};

//FOR COMPLEX PREC
template<FloatLike InputType>
struct console_IO<complex_precision<InputType>> {

	complex_precision<InputType> inline static input(const std::string& var_name = "x");
};

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
		catch (int_precision::bad_int_syntax& e){ error_handler(error_mes_invalid_arg); } 
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

template<FloatLike InputType>
complex_precision<InputType> inline console_IO<complex_precision<InputType>>::input(const std::string& var_name){

    InputType real_x = console_IO<InputType>::input("Re(x)");
    InputType imag_x = console_IO<InputType>::input("Im(x)");

    return complex_precision<InputType>(real_x, imag_x);

}