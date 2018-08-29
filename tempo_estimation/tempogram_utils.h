//
// Created by egordm on 29-8-2018.
//

#ifndef PROJECT_TEMPOGRAM_UTILS_H
#define PROJECT_TEMPOGRAM_UTILS_H

#include <armadillo>

using namespace arma;

namespace tempogram { namespace tempogram_utils {
    mat subtract_mean(const mat &tempogram);

    mat normalize(const mat &tempogram);

    uvec argmax(const mat &tempogram);

    vec max_bucket(const mat &tempogram, const vec &axis_lut);

    vec extract_confidence(const mat &tempogram, const bool &rollover = true);

}}


#endif //PROJECT_TEMPOGRAM_UTILS_H
