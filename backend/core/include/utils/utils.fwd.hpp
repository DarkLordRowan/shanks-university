/**
 * @brief Adapter struct for more convenient integration of third party libraries with some custom integer like type
 *
 * consists of mathematical functions, helpers (isfinite, fma, to_string, ...), functions for acquiring precision if
 * type is able to do it, and setting precision if type is able to do it. If such functionality isn't implemented for
 * given type nothing is done or methods return zero (for now).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

struct utils {
    // utils for math functions
    //template <typename T>
    //struct math;

    // utils for helpers such as isfinite, isnan, to_string etc.
    template <typename T>
    struct helpers;

    // utils for casting
    //template <typename T>
    //struct cast;
};

#include "helpers/utils_helpers.fwd.hpp"