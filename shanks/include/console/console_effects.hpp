#ifndef CONSOLE_EFFECTS_HPP
#define CONSOLE_EFFECTS_HPP

enum class font_style{

};

/**
 * @file console_effects.hpp
 * @brief This file contains utils for work with console.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

// Usefull constants.
#define BOLD            1m
#define DIM             2m
#define ITALIC          3m
#define UNDERLINE       4m
#define BLINKING        5m
#define REVERSE         6m
#define HIDDEN          7m
#define STRIKETHROUGH   8m

#include <cstdio>
#include <iostream>

// Console_effects structure for a better output.
struct console_effects{

	unsigned short int foreground_color;
    unsigned short int background_color;

	void inline static stop_console(){
		#ifdef __unix__
    	system("read -n 1 -s -p \"Press any key to continue...\"");
    	std::cout << std::endl;
    	#endif

    	#ifdef _WIN32
    	system("pause");
    	#endif
	}

	void inline static clear_console()
	{
		#ifdef __unix__
		system("clear");
		#endif
		#ifdef _WIN32
		system("cls");
		#endif
	}

	void inline static clear_line(){
		std::cout << "\x1b[2K\r";
	}

	void inline static clear_to_end(){
		std::cout << "\x1b[0J";
	}

	void inline static move_up(size_t count = static_cast<size_t>(1)){
		std::cout << "\x1b[" << count << "A";
	}

    void inline static move_down(size_t count = static_cast<size_t>(1)){
        std::cout << "\x1b[" << count << "B";
    }

	void inline static clear_lines_up(size_t count){
		clear_line();
		for(size_t j = 0; j < count; ++j){
			move_up(); clear_line();
		}
	}

};

#endif