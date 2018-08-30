//
// Created by egordm on 30-8-2018.
//

#ifndef PROJECT_SIGNAL_UTILS_H
#define PROJECT_SIGNAL_UTILS_H

#include <armadillo>
#include "defines.h"

using namespace arma;

namespace tempogram { namespace signal_utils {
    /**
     * Generates sinus and cosinus pulse for given window and feature rate at given bpm.
     * @param bpm
     * @param window_length
     * @param feature_rate
     * @return
     */
    std::tuple<vec, vec> generate_pulse(double bpm, int window_length, int feature_rate); // TODO: implement in compute_fourier_coefficients
}}


#endif //PROJECT_SIGNAL_UTILS_H
