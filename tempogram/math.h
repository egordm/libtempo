//
// Created by egordm on 25-7-2018.
//

#ifndef TEMPOGRAM_TEMPO_ESTIMATION_MATH_H
#define TEMPOGRAM_TEMPO_ESTIMATION_MATH_H

#include <cmath>

namespace tempogram { namespace math {
    float fixf(float x) {
        return x >= 0 ? floorf(x) : ceilf(x);
    }
}}

#endif //TEMPOGRAM_TEMPO_ESTIMATION_MATH_H
