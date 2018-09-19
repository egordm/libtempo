//
// Created by egordm on 30-8-2018.
//

#ifndef PROJECT_SIGNAL_UTILS_H
#define PROJECT_SIGNAL_UTILS_H

#include <armadillo>
#include "defines.h"

using namespace arma;

namespace libtempo { namespace signal_utils {
    /**
     * Generates sine and consine pulse for given window and feature rate at given bpm.
     * @param bpm
     * @param window_length
     * @param feature_rate
     * @param shift by: number of samples by which the zero of the pulse should be fixed
     * @return
     */
    std::tuple<vec, vec> generate_pulse(double bpm, int window_length, int feature_rate, int shift_by=0);

    fvec generate_click(int sr = 22050, double duration = 0.1, double freq = 1000.0);

    fvec generate_click_track(const std::vector<float> &positions, unsigned long length, int sr = 22050);

    fvec generate_click_track(double bpm, double offset, int note_fraction, unsigned long length, int sr = 22050);
}}


#endif //PROJECT_SIGNAL_UTILS_H
