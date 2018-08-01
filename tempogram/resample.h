//
// Created by egordm on 30-7-2018.
//

#ifndef PROJECT_RESAMPLE_H
#define PROJECT_RESAMPLE_H

#include <armadillo>

using namespace arma;

namespace tempogram {
    vec resample(const vec &signal, int upfactor, int downfactor);
}


#endif //PROJECT_RESAMPLE_H
