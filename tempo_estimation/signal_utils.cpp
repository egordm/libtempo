//
// Created by egordm on 30-8-2018.
//

#include "signal_utils.h"

std::tuple<vec, vec> tempogram::signal_utils::generate_pulse(double bpm, int window_length, int feature_rate) {
    double freq = bpm / 60.;
    vec T = regspace<vec>(0, window_length - 1) / feature_rate;
    vec twoPiFt = freq * 2 * M_PI * T;

    return std::make_tuple(cos(twoPiFt), sin(twoPiFt));
}
