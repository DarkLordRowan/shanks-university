#ifndef CONSOLE_EFFECTS_HPP
#define CONSOLE_EFFECTS_HPP

/**
 * @file console_effects.hpp
 * @brief This file contains utilities for console manipulation and output effects.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

// Usefull constants for terminal text styling
#define BOLD 1m
#define DIM 2m
#define ITALIC 3m
#define UNDERLINE 4m
#define BLINKING 5m
#define REVERSE 6m
#define HIDDEN 7m
#define STRIKETHROUGH 8m

#include <cstdio>
#include <iostream>

/**
 * @brief Console_effects structure for a better output.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
struct console_effects {
    unsigned short int foreground_color;
    unsigned short int background_color;

    /**
     * @brief Stops console and waits for user input
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    void inline static stop_console() {
// Platform-specific pause command execution
#ifdef __unix__
        system("read -n 1 -s -p \"Press any key to continue...\"");
        std::cout << std::endl;
#endif

#ifdef _WIN32
        system("pause");
#endif
    }

    /**
     * @brief Clears the console screen
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    void inline static clear_console() {
// Platform-specific clear screen command execution
#ifdef __unix__
        system("clear");
#endif
#ifdef _WIN32
        system("cls");
#endif
    }

    /**
     * @brief Clears current line in the console
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    void inline static clear_line() {
        // Using ANSI escape sequence to clear line
        std::cout << "\x1b[2K\r";
    }

    /**
     * @brief Clears console from cursor to the end of screen
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    void inline static clear_to_end() {
        // Using ANSI escape sequence to clear to end
        std::cout << "\x1b[0J";
    }

    /**
     * @brief Moves console cursor up
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param count (size_t)
     */
    void inline static move_up(size_t count = static_cast<size_t>(1)) {
        // Using ANSI escape sequence to move cursor up
        std::cout << "\x1b[" << count << "A";
    }

    /**
     * @brief Moves console cursor down
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param count (size_t)
     */
    void inline static move_down(size_t count = static_cast<size_t>(1)) {
        // Using ANSI escape sequence to move cursor down
        std::cout << "\x1b[" << count << "B";
    }

    /**
     * @brief Clears specified number of lines above the current one
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param count (size_t)
     */
    void inline static clear_lines_up(size_t count) {
        // Iteratively moving up and clearing lines
        clear_line();
        for (size_t j = 0; j < count; ++j) {
            move_up();
            clear_line();
        }
    }
};

#endif
