//
// Created by egordm on 26-7-2018.
//

#ifndef PROJECT_MATH_UTILS_HPP
#define PROJECT_MATH_UTILS_HPP

#include <cmath>

namespace tempogram { namespace utils { namespace math {

    inline int fix(double x) {
        return static_cast<int>(x >= 0 ? floor(x) : ceil(x));
    }

}}}

#endif //PROJECT_MATH_UTILS_HPP
