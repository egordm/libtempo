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

    vec generate_click(int sr = 22050, double duration = 0.1, double freq = 1000.0);

    vec generate_click_track(const std::vector<float> &positions, int length, int sr = 22050);

    vec generate_click_track(double bpm, double offset, int note_fraction, int length, int sr = 22050);
}}


#endif //PROJECT_SIGNAL_UTILS_H
